#ifndef MODALITYKINECT_H
#define MODALITYKINECT_H

#if defined(MMR_MODALITY_KINECT)

#include <QBuffer>
#include <QDataStream>
#include <QImage>
#include <QMutex>
#include <QMutexLocker>
#include <QObject>
#include <QThread>

#include <Kinect.h>

#include "modality.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class ModalityKinect;

class KinectThread : public QThread
{
    Q_OBJECT
public:
    KinectThread(QObject *parent = nullptr);
    ~KinectThread();

    ModalityKinect *modality;

    int colorFrameHeight;

    bool initialize();
    void reset();

    bool startAcquisition();
    void stopAcquisition();

signals:
    void acquired(qint64 timestamp, QByteArray data);

protected:
    void run() override;

private:
    QMutex mutex;

    IKinectSensor *sensor = NULL;

    IColorFrameSource *colorFrameSource = NULL;
    IColorFrameReader *colorFrameReader = NULL;

    IDepthFrameSource *depthFrameSource = NULL;
    IDepthFrameReader *depthFrameReader = NULL;

    bool abort;
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class ModalityKinect : public Modality
{
    Q_OBJECT
public:
    explicit ModalityKinect(QObject *parent = nullptr);

    KinectThread kinectThread;

    int colorFrameHeight = 1080;

    bool initialize(QVariantMap configuration) override;
    void reset() override;

    bool startAcquisition() override;
    void stopAcquisition() override;

    QVariantList parseData(QByteArray data) override;

private slots:
    void slotKinectThreadAcquired(qint64 timestamp, QByteArray data);
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#endif // MMR_MODALITY_KINECT

#endif // MODALITYKINECT_H
