#include "quickmain.h"

#include "mmrobject.h"

#include "../shared/irqm/irqmsignalhandler.h"
#include "../shared/irqm/irqmjsonhelper.h"

#include "../shared/mmrfilemetadata.h"

#include "../shared/modality/modality.h"
#include "../shared/modality/modalitykinect.h"
#include "../shared/modality/modalityqtsensor.h"
#include "../shared/modality/modalitybitalino.h"

QuickMain *qMain;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
QuickMainObjectImageProvider::QuickMainObjectImageProvider() : QQuickImageProvider(QQuickImageProvider::Image) {
}
//---------------------------------------------------------------------------
QImage QuickMainObjectImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize) {

    QStringList idParts = id.split("/");
    if (idParts.count() < 2) return QImage();

    int index = idParts[0].toInt();
    QString dataName = idParts[1];

    return qMain->getObjectAt(index)->getModalityImageData(dataName);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
QuickMain::QuickMain(QObject *parent) : QObject(parent) {
    objectImageProvider = new QuickMainObjectImageProvider();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void QuickMain::loadMMRData(QString basePath) {
    unloadMMRData();

    fileMetadata = new MMRFileMetadata(this);
    fileMetadata->loadFromFileDirPath(basePath);

    createMMRObjects();

    IRQMSignalHandler::sendSignal("main", "dataLoaded");
}
//---------------------------------------------------------------------------
void QuickMain::unloadMMRData() {
    destroyMMRObjects();

    if (fileMetadata) {
        fileMetadata->deleteLater();
        fileMetadata = NULL;
    }

    IRQMSignalHandler::sendSignal("main", "dataUnloaded");
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
MMRFileMetadata *QuickMain::getFileMetadata() {
    return fileMetadata;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int QuickMain::getNumObjects() {
    return objectList.size();
}
//---------------------------------------------------------------------------
MMRObject *QuickMain::getObjectAt(int index) {
    return objectList.at(index);
}
//---------------------------------------------------------------------------
void QuickMain::loadObjectData(qint64 timestamp) {
    QMutexLocker loadObjectDataMutexLocker(&loadObjectDataMutex);

    if (!fileMetadata) return;

    for (auto object=objectList.begin(); object!=objectList.end(); ++object) {
        QString identifier = (*object)->identifier;

        qint64 dataPos = fileMetadata->getModalityDataPos(identifier, timestamp);
        (*object)->loadModalityData(dataPos);
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void QuickMain::createMMRObjects() {
    if (!fileMetadata) return;

    QVariantList modalities = fileMetadata->getModalities();

    for (auto modalityVariant=modalities.begin(); modalityVariant!=modalities.end(); ++modalityVariant) {
        QVariantMap modalityInfo = (*modalityVariant).toMap();

        MMRObject *object = new MMRObject();
        object->setModalityInfo(modalityInfo);
        object->loadFileData(fileMetadata->basePath);

        objectList.append(object);
    }
}
//---------------------------------------------------------------------------
void QuickMain::destroyMMRObjects() {
    while (objectList.count() > 0) {
        MMRObject *object = objectList.takeLast();
        object->clear();
        object->deleteLater();
    }

    if (fileMetadata) {
        fileMetadata->clear();
        fileMetadata->deleteLater();
        fileMetadata = NULL;
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
qint64 QuickMain::getCurrentTimestamp() {
    return QDateTime::currentMSecsSinceEpoch();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void QuickMain::writeBlobData() {
    if (!fileMetadata) return;

    fileMetadata->prepareWritingBlobData();

    for (auto object=objectList.begin(); object!=objectList.end(); ++object) {
        QString identifier = (*object)->identifier;
        QString modalityType = (*object)->modality->type;

        QVariantList recordings = fileMetadata->getModalityRecordings(identifier);

        fileMetadata->beginWriteBlobDataTransaction();

        for (int i=0; i<recordings.count(); i++) {
            qDebug() << identifier << i << recordings.count();
            QVariantMap recording = recordings.at(i).toMap();

            int recordingIdx = recording.value("recording_idx").toInt();
            qint64 dataPos = recording.value("data_pos").toLongLong();

            (*object)->loadModalityData(dataPos);

            if (modalityType == "kinect") {
                QImage colorImage = (*object)->getModalityImageData("color");
                colorImage = colorImage.scaledToHeight(360);
                QByteArray colorByteArray;
                QBuffer colorBuffer(&colorByteArray);
                colorBuffer.open(QIODevice::WriteOnly);
                colorImage.save(&colorBuffer, "PNG");
                fileMetadata->writeBlobData(recordingIdx, "color", colorByteArray);

                QImage depthImage = (*object)->getModalityImageData("depth");
                QByteArray depthByteArray;
                QBuffer depthBuffer(&depthByteArray);
                depthBuffer.open(QIODevice::WriteOnly);
                depthImage.save(&depthBuffer, "PNG");
                fileMetadata->writeBlobData(recordingIdx, "depth", depthByteArray);
            }
            else if (modalityType == "qtsensor") {
                QString dataType = (*object)->loadedData.at(0).toMap().value("value").toString();

                QVariantList dataList;
                for (int j=1; j<(*object)->loadedData.count(); j++) {
                    dataList.append((*object)->loadedData.at(j).toMap().value("value"));
                }
                QByteArray data = IRQMJSONHelper::JSONObjectToByteArray(dataList);
                fileMetadata->writeBlobData(recordingIdx, dataType, data);
            }
        }

        fileMetadata->commitWriteBlobDataTransaction();
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
