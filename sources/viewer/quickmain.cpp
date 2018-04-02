#include "quickmain.h"

#include "mmrobject.h"

#include "../shared/irqm/irqmsignalhandler.h"

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
    if (fileMetadata) {
        fileMetadata->deleteLater();
        fileMetadata = NULL;
    }
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
//---------------------------------------------------------------------------
qint64 QuickMain::getCurrentTimestamp() {
    return QDateTime::currentMSecsSinceEpoch();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
