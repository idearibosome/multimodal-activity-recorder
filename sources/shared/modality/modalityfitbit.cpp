#if defined(MMR_MODALITY_FITBIT)

#include "modalityfitbit.h"

#include "shared/irqm/irqmjsonhelper.h"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ModalityFitbit::ModalityFitbit(QObject *parent) : Modality(parent) {
    type = "fitbit";

    isInAcquisition = false;
    httpServer = nullptr;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool ModalityFitbit::initialize(QVariantMap configuration) {
    return Modality::initialize(configuration);
}
//---------------------------------------------------------------------------
void ModalityFitbit::reset() {
    Modality::reset();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool ModalityFitbit::startAcquisition() {
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
QVariantList ModalityFitbit::parseData(QByteArray data) {
    QVariantList parsedDataList;

    QDataStream inStream(&data, QIODevice::ReadOnly);
    inStream.setVersion(QDataStream::Qt_5_9);

    QVariantMap parsedData;
    inStream >> parsedData;

    if (parsedData.contains("accel")) {
        QVariantMap currentMap = parsedData.value("accel").toMap();
        parsedDataList.append(Modality::parsedDataItemWithValue("acc_x", currentMap.value("x")));
        parsedDataList.append(Modality::parsedDataItemWithValue("acc_y", currentMap.value("y")));
        parsedDataList.append(Modality::parsedDataItemWithValue("acc_z", currentMap.value("z")));
    }
    if (parsedData.contains("bar")) {
        QVariantMap currentMap = parsedData.value("bar").toMap();
        parsedDataList.append(Modality::parsedDataItemWithValue("pressure", currentMap.value("pressure")));
    }
    if (parsedData.contains("bps")) {
        QVariantMap currentMap = parsedData.value("bps").toMap();
        parsedDataList.append(Modality::parsedDataItemWithValue("body_presence", currentMap.value("presence")));
    }
    if (parsedData.contains("gyro")) {
        QVariantMap currentMap = parsedData.value("gyro").toMap();
        parsedDataList.append(Modality::parsedDataItemWithValue("gyro_x", currentMap.value("x")));
        parsedDataList.append(Modality::parsedDataItemWithValue("gyro_y", currentMap.value("y")));
        parsedDataList.append(Modality::parsedDataItemWithValue("gyro_z", currentMap.value("z")));
    }
    if (parsedData.contains("hrm")) {
        QVariantMap currentMap = parsedData.value("hrm").toMap();
        parsedDataList.append(Modality::parsedDataItemWithValue("heart_rate", currentMap.value("heartRate")));
    }
    if (parsedData.contains("orientation")) {
        QVariantList currentList = parsedData.value("orientation").toList();
        parsedDataList.append(Modality::parsedDataItemWithValue("ori_scalar", currentList[0]));
        parsedDataList.append(Modality::parsedDataItemWithValue("ori_i", currentList[1]));
        parsedDataList.append(Modality::parsedDataItemWithValue("ori_j", currentList[2]));
        parsedDataList.append(Modality::parsedDataItemWithValue("ori_k", currentList[3]));
    }

    return parsedDataList;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void ModalityFitbit::startServer(int port) {
    stopServer();

    httpServer = new qhttp::server::QHttpServer();
    httpServer->listen(QHostAddress::Any, port, [&](qhttp::server::QHttpRequest *req, qhttp::server::QHttpResponse *res) {
        req->collectData();
        req->onEnd([this, req, res]() {
            if (this->isInAcquisition) {
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
    qDebug() << data;

    QVariantMap parsedData = IRQMJSONHelper::byteArrayToJSONObject(data).toMap();
    if (parsedData.isEmpty()) return;

    qint64 timestamp = getAcquisitionTimestamp();

    QByteArray byteArray;
    QDataStream outStream(&byteArray, QIODevice::WriteOnly);
    outStream.setVersion(QDataStream::Qt_5_9);

    outStream << parsedData;

    emit acquired(timestamp, byteArray);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#endif
