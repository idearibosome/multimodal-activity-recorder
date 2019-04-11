#include "mmrrecognizerconnection.h"

#include "../shared/irqm/irqmjsonhelper.h"
#include "../shared/irqm/irqmsignalhandler.h"

#include "../shared/modality/modality.h"
#include "../shared/modality/modalityconfigurator.h"

#include "../shared/mmrwsdata.h"

#include "mmrserver.h"
#include "mmrmodalityconnection.h"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
MMRRecognizerConnection::MMRRecognizerConnection(QObject *parent) : QObject(parent) {
    name = "";
    identifier = "";
    fps = 0;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRRecognizerConnection::slotWsBinaryMessageReceived(QByteArray message) {
    MMRWSData *wsData = new MMRWSData();
    wsData->loadFromByteArray(message);

    if (wsData->dataType == "request") {
        handleRequest(wsData);
    }
    else if (wsData->dataType == "response") {
        handleResponse(wsData);
    }
}
//---------------------------------------------------------------------------
void MMRRecognizerConnection::slotWsDisconnected() {
    if (this->identifier != "") {
        log("disconnected (" +  this->identifier + ")");
        IRQMSignalHandler::sendSignal("mmrconnection", "recognizerDisconnected", this->name, this->identifier);
    }

    finalize();

    emit disconnected();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRRecognizerConnection::slotPrepare() {
    prepare();
}
//---------------------------------------------------------------------------
void MMRRecognizerConnection::slotStart() {
    start();
}
//---------------------------------------------------------------------------
void MMRRecognizerConnection::slotStop() {
    stop();
}
//---------------------------------------------------------------------------
void MMRRecognizerConnection::slotFinalize() {
    finalize();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRRecognizerConnection::slotRecognizerTimerFired() {
    recognize();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRRecognizerConnection::log(QString text) {
    IRQMSignalHandler::sendSignal("main", "log", "[" + this->name + ", " + identifier.left(6) + "] " + text);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRRecognizerConnection::setWebSocket(QWebSocket *ws) {
    this->ws = ws;

    QObject::connect(ws, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(slotWsBinaryMessageReceived(QByteArray)), Qt::QueuedConnection);
    QObject::connect(ws, SIGNAL(disconnected()), this, SLOT(slotWsDisconnected()), Qt::QueuedConnection);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRRecognizerConnection::prepare() {
    MMRWSData *data = new MMRWSData();
    data->requestType = "prepare";
    data->dataType = "request";

    sendRequest(data);

    data->deleteLater();
}
//---------------------------------------------------------------------------
void MMRRecognizerConnection::start() {
    MMRWSData *data = new MMRWSData();
    data->requestType = "start";
    data->dataType = "request";

    sendRequest(data);

    data->deleteLater();

    // start timer
    recognizerTimer = new QTimer(this);
    QObject::connect(recognizerTimer, SIGNAL(timeout()), this, SLOT(slotRecognizerTimerFired()));
    int msec = 1000 / fps;
    recognizerTimer->start(msec);
}
//---------------------------------------------------------------------------
void MMRRecognizerConnection::recognize() {
    if (!server) return;

    QVariantMap recognitionData;

    foreach (QString modalityIdentifier, registeredModalityList) {
        MMRModalityConnection *connection = server->modalityConnectionForIdentifier(identifier);
        if (!connection) continue;
        if (connection->lastData.isEmpty()) continue;

        Modality *modality = ModalityConfigurator::modalityForType(connection->type);
        if (!modality) continue;

        recognitionData.insert(modalityIdentifier, modality->parseData(connection->lastData, Modality::ParseType_Recognizer));
    }

    MMRWSData *data = new MMRWSData();
    data->requestType = "recognize";
    data->dataType = "request";
    data->data = recognitionData;

    sendRequest(data);

    data->deleteLater();
}
//---------------------------------------------------------------------------
void MMRRecognizerConnection::stop() {
    // stop timer
    if (recognizerTimer) {
        recognizerTimer->stop();
        recognizerTimer->deleteLater();
        recognizerTimer = nullptr;
    }

    MMRWSData *data = new MMRWSData();
    data->requestType = "stop";
    data->dataType = "request";

    sendRequest(data);

    data->deleteLater();
}
//---------------------------------------------------------------------------
void MMRRecognizerConnection::finalize() {
    MMRWSData *data = new MMRWSData();
    data->requestType = "finalize";
    data->dataType = "request";

    sendRequest(data);

    data->deleteLater();

    identifier = "";
}
//---------------------------------------------------------------------------
void MMRRecognizerConnection::close() {
    if (this->ws) {
        emit closeWebSocket(this->ws);
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRRecognizerConnection::handleRequest(MMRWSData *wsData) {
    if (!ws) return;

    QString type = wsData->requestType;
    QVariantMap data = wsData->data;

    if (type == "register") handleRequestRegister(type, data);

    wsData->deleteLater();
}
//---------------------------------------------------------------------------
void MMRRecognizerConnection::handleRequestRegister(QString type, QVariantMap data) {
    this->name = data.value("name").toString();
    this->identifier = data.value("identifier").toString();
    this->registeredModalityList = data.value("modalities").toStringList();
    this->fps = data.value("fps").toDouble();

    log("registered (" + this->name + ", " + this->identifier + ")");
    IRQMSignalHandler::sendSignal("mmrconnection", "recognizerRegistered", this->name, this->identifier);

    MMRWSData wsData;
    wsData.requestType = type;
    wsData.dataType = "response";
    wsData.data.insert("result", QString("ok"));

    emit sendBinaryMessage(ws, wsData.toByteArray());
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRRecognizerConnection::handleResponse(MMRWSData *wsData) {
    if (!ws) return;

    QString type = wsData->requestType;
    QVariantMap data = wsData->data;

    if (type == "recognize") handleResponseRecognize(type, data);

    wsData->deleteLater();
}
//---------------------------------------------------------------------------
void MMRRecognizerConnection::handleResponseRecognize(QString type, QVariantMap data) {
    QString dataString = IRQMJSONHelper::JSONObjectToString(data);

    log("recognized (" + this->name + ", " + this->identifier + ") - " + dataString);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRRecognizerConnection::sendRequest(MMRWSData *wsData) {
    if (!ws) return;

    QByteArray message = wsData->toByteArray();
    emit sendBinaryMessage(ws, message);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
