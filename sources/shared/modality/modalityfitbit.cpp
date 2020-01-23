#if defined(MMR_MODALITY_FITBIT)

#include "modalityfitbit.h"

#include "shared/irqm/irqmjsonhelper.h"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ModalityFitbit::ModalityFitbit(QObject *parent) : Modality(parent) {
    type = "fitbit";

    isInitialized = false;
    isInAcquisition = false;
    httpServer = nullptr;
    minimumTimestampDiff = -1;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool ModalityFitbit::initialize(QVariantMap configuration) {
    minimumTimestampDiff = -1;
    isInitialized = true;

    return Modality::initialize(configuration);
}
//---------------------------------------------------------------------------
void ModalityFitbit::reset() {
    isInitialized = false;
    minimumTimestampDiff = -1;

    Modality::reset();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool ModalityFitbit::startAcquisition() {
    if (!isInitialized) return false;
    if (!httpServer) return false;

    startRecordingAcquisitionTimestamp();
    isInAcquisition = true;

    return true;
}
//---------------------------------------------------------------------------
void ModalityFitbit::stopAcquisition() {
    isInAcquisition = false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void ModalityFitbit::startServer(int port) {
    stopServer();

    httpServer = new qhttp::server::QHttpServer();
    httpServer->listen(QHostAddress::Any, port, [&](qhttp::server::QHttpRequest *req, qhttp::server::QHttpResponse *res) {
        req->collectData();
        req->onEnd([this, req, res]() {
            if (this->isInitialized) {
                this->processAcquiredData(req->collectedData().constData());
            }
            res->setStatusCode(qhttp::ESTATUS_OK);
            res->end();
        });
    });
}
//---------------------------------------------------------------------------
void ModalityFitbit::stopServer() {
    if (!httpServer) return;
    httpServer->deleteLater();
    httpServer = nullptr;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void ModalityFitbit::processAcquiredData(QByteArray data) {
    QVariantMap parsedData = IRQMJSONHelper::byteArrayToJSONObject(data).toMap();
    if (parsedData.isEmpty()) return;

    qint64 fitbitTimestamp = parsedData.value("ts").toLongLong();
    qint64 timestamp = getTimestamp();

    qint64 timestampDiff = timestamp - fitbitTimestamp;
    if (minimumTimestampDiff < 0 || timestampDiff < minimumTimestampDiff) {
        minimumTimestampDiff = timestampDiff;
    }
    timestamp = fitbitTimestamp + minimumTimestampDiff;

    parsedData.remove("ts");

    if (this->isInAcquisition) {
        qint64 acquisitionTimestamp = getAcquisitionTimestamp(timestamp);

        QByteArray byteArray;
        QDataStream outStream(&byteArray, QIODevice::WriteOnly);
        outStream.setVersion(QDataStream::Qt_5_9);

        outStream << parsedData;

        emit acquired(acquisitionTimestamp, byteArray);
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#endif
