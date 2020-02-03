#include "quickmain.h"

#include "mmrobject.h"

#include "../shared/irqm/irqmsignalhandler.h"
#include "../shared/irqm/irqmpathhelper.h"

#include "../shared/mmrfilemetadata.h"

#include "../shared/modality/parser/modalityparser.h"

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
void QuickMain::exportMMRData(QString exportPath) {
    if (!fileMetadata) return;

    QtConcurrent::run(this, &QuickMain::doExportMMRData, exportPath);
}
//---------------------------------------------------------------------------
void QuickMain::doExportMMRData(QString exportPath) {
    if (!fileMetadata) return;

    for (auto object=objectList.begin(); object!=objectList.end(); ++object) {
        QString identifier = (*object)->identifier;
        QString modalityType = (*object)->modalityParser->type;

        QVariantList recordings = fileMetadata->getModalityRecordings(identifier);

        IRQMSignalHandler::sendSignal("main", "exportLog", QString("[%1] %2, %3 recording(s)").arg(identifier).arg(modalityType).arg(recordings.count()));

        QString modalityPath = IRQMPathHelper::concatenate(exportPath, modalityType+"_"+identifier);
        QDir modalityPathDir(modalityPath);
        if (!modalityPathDir.exists()) {
            modalityPathDir.mkpath(".");
        }

        QString csvPath = IRQMPathHelper::concatenate(modalityPath, "data.csv");
        QFile csvFile(csvPath);
        csvFile.open(QFile::WriteOnly);

        for (int i=0; i<recordings.count(); i++) {
            if (i % 100 == 99) {
                IRQMSignalHandler::sendSignal("main", "exportLog", QString("[%1] exporting %2 of %3").arg(identifier).arg(i+1).arg(recordings.count()));
            }
            QVariantMap recording = recordings.at(i).toMap();

            int recordingIdx = recording.value("recording_idx").toInt();
            qint64 dataPos = recording.value("data_pos").toLongLong();
            qint64 timestamp = recording.value("timestamp").toLongLong();

            (*object)->loadModalityData(dataPos);

            QVariantList dataList = (*object)->getModalityDataList();

            // initial configuration: csv header, image directory
            if (i == 0) {
                QStringList csvHeaderList;
                csvHeaderList.append("recording_idx");
                csvHeaderList.append("timestamp");
                for (int dataIndex=0; dataIndex<dataList.count(); dataIndex++) {
                    QVariantMap data = dataList[dataIndex].toMap();
                    QString dataName = data.value("name").toString();
                    QString dataType = data.value("type").toString();

                    if (dataType == "value") {
                        csvHeaderList.append(dataName);
                    }
                    else if (dataType == "image") {
                        QString modalityImagePath = IRQMPathHelper::concatenate(modalityPath, dataName);
                        QDir modalityImagePathDir(modalityImagePath);
                        if (!modalityImagePathDir.exists()) {
                            modalityImagePathDir.mkpath(".");
                        }
                    }
                }
                csvFile.write(QString(csvHeaderList.join(",")+"\n").toUtf8());
            }

            // data
            QStringList csvDataList;
            csvDataList.append(QString("\"%1\"").arg(recordingIdx));
            csvDataList.append(QString("\"%1\"").arg(timestamp));

            for (int dataIndex=0; dataIndex<dataList.count(); dataIndex++) {
                QVariantMap data = dataList[dataIndex].toMap();
                QString dataName = data.value("name").toString();
                QString dataType = data.value("type").toString();

                if (dataType == "value") {
                    csvDataList.append(QString("\"%1\"").arg(data.value("value").toString()));
                }
                else if (dataType == "image") {
                    QString modalityImagePath = IRQMPathHelper::concatenate(modalityPath, dataName);
                    QImage modalityImage = (*object)->getModalityImageData(dataName);
                    modalityImage.save(IRQMPathHelper::concatenate(modalityImagePath, QString("%1.png").arg(timestamp)));
                }
            }

            csvFile.write(QString(csvDataList.join(",")+"\n").toUtf8());
        }

        csvFile.close();
        IRQMSignalHandler::sendSignal("main", "exportLog", QString("[%1] finished").arg(identifier));
    }

    IRQMSignalHandler::sendSignal("main", "exportLog", QString("finished"));
    IRQMSignalHandler::sendSignal("main", "exportFinished");
}
//---------------------------------------------------------------------------
