#include "modalitymmrfile.h"

#include "../../shared/mmrfiledata.h"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ModalityMMRFile::ModalityMMRFile(QObject *parent) : Modality(parent) {
    type = "";
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool ModalityMMRFile::initialize(QVariantMap configuration) {
    pendingRecordings = recordings;

    // file data
    if (!filePath.isEmpty()) {
        fileData = new MMRFileData(this);
        fileData->loadFromFilePath(filePath);
    }

    // timer
    dataTimer = new QTimer(this);
    dataTimer->setInterval(50);
    QObject::connect(dataTimer, SIGNAL(timeout()), this, SLOT(slotDataTimerTimeout()));

    return Modality::initialize(configuration);
}
//---------------------------------------------------------------------------
void ModalityMMRFile::reset() {
    pendingRecordings.clear();

    if (dataTimer) {
        dataTimer->stop();
        dataTimer->deleteLater();
        dataTimer = nullptr;
    }

    if (fileData) {
        fileData->clear();
        fileData->deleteLater();
        fileData = nullptr;
    }

    Modality::reset();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void ModalityMMRFile::slotDataTimerTimeout() {
    if (!fileData) return;

    qint64 currentTimestamp = getAcquisitionTimestamp();

    while (pendingRecordings.size() > 0) {
        QVariantMap recording = pendingRecordings.at(0).toMap();

        qint64 timestamp = recording.value("timestamp").toLongLong();
        if (currentTimestamp < timestamp) break;

        qint64 dataPos = recording.value("data_pos").toLongLong();

        QVariantMap timestampAndData = fileData->getData(dataPos);
        timestamp = timestampAndData.value("timestamp").toLongLong();
        QByteArray data = timestampAndData.value("data").toByteArray();

        emit acquired(timestamp, data);
        pendingRecordings.removeAt(0);
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool ModalityMMRFile::startAcquisition() {
    startRecordingAcquisitionTimestamp();

    if (dataTimer) dataTimer->start();

    return true;
}
//---------------------------------------------------------------------------
void ModalityMMRFile::stopAcquisition() {
    if (dataTimer) dataTimer->stop();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
