#include "mmrclient.h"

#include "../shared/modality/modality.h"

#include "../shared/irqm/irqmsignalhandler.h"

#include "../shared/mmrwsdata.h"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
MMRClient::MMRClient(QObject *parent) : QObject(parent) {
    this->identifier = QUuid::createUuid().toString().replace(QRegularExpression("[^0-9A-Za-z]"), "");

    ws = new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this);

    QObject::connect(ws, SIGNAL(connected()), this, SLOT(slotWsConnected()));
    QObject::connect(ws, SIGNAL(disconnected()), this, SLOT(slotWsDisconnected()));
    QObject::connect(ws, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotWsError(QAbstractSocket::SocketError)));
    QObject::connect(ws, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(slotWsBinaryMessageReceived(QByteArray)));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRClient::slotWsConnected() {
    this->log(QString("ws: Connected (%1)").arg(ws->requestUrl().toString()));

    IRQMSignalHandler::sendSignal("mmrclient", "serverConnected", identifier);

    requestRegister();
}
//---------------------------------------------------------------------------
void MMRClient::slotWsDisconnected() {
    this->log("ws: Disconnected");

    {
        QMutexLocker modalityDataMutexLocker(&modalityDataMutex);
        isWsReadyToReceiveModalityData = false;
    }

    IRQMSignalHandler::sendSignal("mmrclient", "serverDisconnected", identifier);
}
//---------------------------------------------------------------------------
void MMRClient::slotWsError(QAbstractSocket::SocketError error) {
    this->log("ws: Error - " + ws->errorString());

    IRQMSignalHandler::sendSignal("mmrclient", "serverError", identifier, (int)error, ws->errorString());
}
//---------------------------------------------------------------------------
void MMRClient::slotWsBinaryMessageReceived(QByteArray message) {
    MMRWSData *wsData = new MMRWSData();
    wsData->loadFromByteArray(message);

    if (wsData->dataType == "request") {
        handleRequest(wsData);
    }
    if (wsData->dataType == "response") {
        handleResponse(wsData);
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRClient::slotModalityAcquired(qint64 timestamp, QByteArray data) {

    {
        QMutexLocker modalityDataMutexLocker(&modalityDataMutex);

        QVariantMap pendingModalityData;
        pendingModalityData.insert("timestamp", timestamp);
        pendingModalityData.insert("data", data);

        pendingModalityDataList.append(pendingModalityData);
        hasPendingModalityData = true;
    }

    sendPendingModalityData();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRClient::sendPendingModalityData() {

    QMutexLocker modalityDataMutexLocker(&modalityDataMutex);

    if (!isWsReadyToReceiveModalityData) return;
    if (!hasPendingModalityData) return;

    MMRWSData wsData;
    wsData.requestType = "data_list";
    wsData.dataType = "request";
    wsData.data.insert("list", pendingModalityDataList);

    ws->sendBinaryMessage(wsData.toByteArray());

    isWsReadyToReceiveModalityData = false;
    hasPendingModalityData = false;
    pendingModalityDataList.clear();

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRClient::log(QString text) {
    IRQMSignalHandler::sendSignal("main", "log", "[" + identifier.left(6) + "] " + text);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRClient::setConfiguration(QString key, QVariant value) {
    configuration.insert(key, value);
    qDebug() << identifier << key << value;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRClient::registerModality(Modality *modality) {
    modality->setParent(this);
    modality->identifier = identifier;
    this->modality = modality;

    QObject::connect(modality, SIGNAL(acquired(qint64,QByteArray)), this, SLOT(slotModalityAcquired(qint64,QByteArray)));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRClient::connectServer(QString url) {
    if (ws->isValid()) return;

    this->log(QString("ws: Connecting (%1)").arg(url));

    ws->open(QUrl(url));
}
//---------------------------------------------------------------------------
void MMRClient::disconnectServer() {
    ws->close();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRClient::requestRegister() {
    if (!ws->isValid()) return;

    MMRWSData wsData;
    wsData.requestType = "register";
    wsData.dataType = "request";
    wsData.data.insert("type", modality->type);
    wsData.data.insert("identifier", identifier);

    ws->sendBinaryMessage(wsData.toByteArray());
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRClient::handleRequest(MMRWSData *wsData) {
    if (!ws) return;

    QString type = wsData->requestType;
    QVariantMap data = wsData->data;

    if (type == "prepare") handleRequestPrepare(type, data);
    else if (type == "start") handleRequestStart(type, data);
    else if (type == "stop") handleRequestStop(type, data);
    else if (type == "finalize") handleRequestFinalize(type, data);

    wsData->deleteLater();
}
//---------------------------------------------------------------------------
void MMRClient::handleRequestPrepare(QString type, QVariantMap data) {
    if (!modality) return;
    this->log("Prepare modality");

    MMRWSData wsData;
    wsData.requestType = type;
    wsData.dataType = "response";

    bool res = modality->initialize(configuration);
    if (res) {
        wsData.data.insert("result", "ok");
    }
    else {
        wsData.data.insert("result", "error");
    }

    ws->sendBinaryMessage(wsData.toByteArray());
}
//---------------------------------------------------------------------------
void MMRClient::handleRequestStart(QString type, QVariantMap data) {
    if (!modality) return;
    this->log("Start acquisition");

    isWsReadyToReceiveModalityData = true;
    hasPendingModalityData = false;
    modality->startAcquisition();
}
//---------------------------------------------------------------------------
void MMRClient::handleRequestStop(QString type, QVariantMap data) {
    if (!modality) return;
    this->log("Stop acquisition");

    modality->stopAcquisition();
}
//---------------------------------------------------------------------------
void MMRClient::handleRequestFinalize(QString type, QVariantMap data) {
    if (!modality) return;
    this->log("Finalize modality");

    disconnectServer();

    modality->reset();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRClient::handleResponse(MMRWSData *wsData) {
    if (!ws) return;

    QString type = wsData->requestType;
    QVariantMap data = wsData->data;

    if (type == "register") handleResponseRegister(type, data);
    else if (type == "data") handleResponseData(type, data);
    else if (type == "data_list") handleResponseDataList(type, data);

    wsData->deleteLater();
}
//---------------------------------------------------------------------------
void MMRClient::handleResponseRegister(QString type, QVariantMap data) {
    if (data.value("result").toString() == "ok") {
        this->log("ws: Registered");
    }
    else {
        this->log("ws: Failed to register");
        disconnectServer();
    }
}
//---------------------------------------------------------------------------
void MMRClient::handleResponseData(QString type, QVariantMap data) {
    if (data.value("result").toString() == "ok") {
        QMutexLocker modalityDataMutexLocker(&modalityDataMutex);

        isWsReadyToReceiveModalityData = true;
    }
}
//---------------------------------------------------------------------------
void MMRClient::handleResponseDataList(QString type, QVariantMap data) {
    if (data.value("result").toString() == "ok") {
        {
            QMutexLocker modalityDataMutexLocker(&modalityDataMutex);
            isWsReadyToReceiveModalityData = true;
        }
        sendPendingModalityData();
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
