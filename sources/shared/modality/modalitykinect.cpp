#if defined(MMR_MODALITY_KINECT)

#include "modalitykinect.h"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
KinectThread::KinectThread(QObject *parent) : QThread(parent) {
    abort = false;
}
//---------------------------------------------------------------------------
KinectThread::~KinectThread() {
    reset();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool KinectThread::initialize() {
    HRESULT hResult;

    hResult = GetDefaultKinectSensor(&sensor);
    if (FAILED(hResult)) {
        qDebug() << "Kinect: failed - GetDefaultKinectSensor";
        sensor = NULL;
        return false;
    }

    hResult = sensor->Open();
    if (FAILED(hResult)) {
        qDebug() << "Kinect: failed - IKinectSensor::Open";
        reset();
        return false;
    }

    // color frame
    hResult = sensor->get_ColorFrameSource(&colorFrameSource);
    if (FAILED(hResult)) {
        qDebug() << "Kinect: failed - IKinectSensor::get_ColorFrameSource";
        reset();
        return false;
    }
    hResult = colorFrameSource->OpenReader(&colorFrameReader);
    if (FAILED(hResult)) {
        qDebug() << "Kinect: failed - IColorFrameSource::OpenReader";
        reset();
        return false;
    }

    // depth frame
    hResult = sensor->get_DepthFrameSource(&depthFrameSource);
    if (FAILED(hResult)) {
        qDebug() << "Kinect: failed - IKinectSensor::get_DepthFrameSource";
        reset();
        return false;
    }
    hResult = depthFrameSource->OpenReader(&depthFrameReader);
    if (FAILED(hResult)) {
        qDebug() << "Kinect: failed - IDepthFrameSource::OpenReader";
        reset();
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void KinectThread::reset() {
    mutex.lock();
    abort = true;
    mutex.unlock();

    wait();

    if (colorFrameReader) {
        colorFrameReader->Release();
        colorFrameReader = NULL;
    }
    if (colorFrameSource) {
        colorFrameSource->Release();
        colorFrameSource = NULL;
    }
    if (depthFrameReader) {
        depthFrameReader->Release();
        depthFrameReader = NULL;
    }
    if (depthFrameSource) {
        depthFrameSource->Release();
        depthFrameSource = NULL;
    }
    if (sensor) {
        sensor->Close();
        sensor = NULL;
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool KinectThread::startAcquisition() {
    if (!colorFrameReader) return false;
    if (!depthFrameReader) return false;

    QMutexLocker locker(&mutex);

    if (!isRunning()) {
        modality->startRecordingAcquisitionTimestamp();
        start();
    }

    return true;
}
//---------------------------------------------------------------------------
void KinectThread::stopAcquisition() {
    mutex.lock();
    abort = true;
    mutex.unlock();

    wait();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void KinectThread::run() {
    HRESULT hResult;

    unsigned int colorFrameBufferSize = 1920 * 1080 * 4 * sizeof(unsigned char);
    unsigned char *colorFrameBuffer = new unsigned char[colorFrameBufferSize];
    bool hasColorFrame = false;

    unsigned int depthFrameBufferSize = 0;
    unsigned short *depthFrameBuffer = 0;
    bool hasDepthFrame = false;

    while (true) {
        if (abort) break;

        IColorFrame *colorFrame = NULL;
        IDepthFrame *depthFrame = NULL;

        hasColorFrame = false;
        hasDepthFrame = false;

        do {
            hResult = colorFrameReader->AcquireLatestFrame(&colorFrame);
            if (SUCCEEDED(hResult)) {
                hResult = colorFrame->CopyConvertedFrameDataToArray(colorFrameBufferSize, (BYTE *)colorFrameBuffer, ColorImageFormat_Rgba);
                if (SUCCEEDED(hResult)) {
                    hasColorFrame = true;
                }
            }
        } while (hResult == E_PENDING);
        do {
            hResult = depthFrameReader->AcquireLatestFrame(&depthFrame);
            if (SUCCEEDED(hResult)) {
                hResult = depthFrame->AccessUnderlyingBuffer(&depthFrameBufferSize, (UINT16 **)&depthFrameBuffer);
                if (SUCCEEDED(hResult)) {
                    hasDepthFrame = true;
                }
            }
        } while (hResult == E_PENDING);

        if (hasColorFrame && hasDepthFrame) {
            qint64 timestamp = modality->getAcquisitionTimestamp();

            int colorFrameWidth = (colorFrameHeight * 16 / 9);

            QImage colorImage = QImage(colorFrameBuffer, 1920, 1080, QImage::Format_RGBA8888);
            QImage scaledColorImage = colorImage.scaled(colorFrameWidth, colorFrameHeight);
            scaledColorImage = scaledColorImage.convertToFormat(QImage::Format_RGB888);
            QByteArray colorData((const char *)scaledColorImage.bits(), scaledColorImage.byteCount());

            static int depthFrameWidth = 512;
            static int depthFrameHeight = 424;

            QByteArray depthData((const char *)depthFrameBuffer, depthFrameBufferSize * sizeof(unsigned short));

            QByteArray byteArray;
            QDataStream outStream(&byteArray, QIODevice::WriteOnly);
            outStream.setVersion(QDataStream::Qt_5_9);

            outStream << timestamp;
            outStream << colorFrameWidth << colorFrameHeight << colorData;
            outStream << depthFrameWidth << depthFrameHeight << depthData;

            if (abort) break;

            emit acquired(byteArray);
        }

        if (colorFrame) {
            colorFrame->Release();
        }
        if (depthFrame) {
            depthFrame->Release();
        }
    }

    delete colorFrameBuffer;
    // do not delete depthFrameBuffer, since it is handled by depthFrameReader
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ModalityKinect::ModalityKinect(QObject *parent) : Modality(parent) {
    type = "kinect";

    kinectThread.modality = this;

    QObject::connect(&kinectThread, SIGNAL(acquired(QByteArray)), this, SLOT(slotKinectThreadAcquired(QByteArray)));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool ModalityKinect::initialize(QVariantMap configuration) {
    colorFrameHeight = configuration.value("color_height").toInt();

    bool res = kinectThread.initialize();
    if (!res) return false;

    kinectThread.colorFrameHeight = colorFrameHeight;

    return Modality::initialize(configuration);
}
//---------------------------------------------------------------------------
void ModalityKinect::reset() {
    kinectThread.reset();

    Modality::reset();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void ModalityKinect::slotKinectThreadAcquired(QByteArray data) {
    emit acquired(data);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool ModalityKinect::startAcquisition() {
    return kinectThread.startAcquisition();
}
//---------------------------------------------------------------------------
void ModalityKinect::stopAcquisition() {
    return kinectThread.stopAcquisition();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#endif // MMR_MODALITY_KINECT
