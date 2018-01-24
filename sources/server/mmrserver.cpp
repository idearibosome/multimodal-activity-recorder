#include "mmrserver.h"

#include "mmrconnection.h"

#include "../shared/irqm/irqmsignalhandler.h"

#include "../shared/mmrwsdata.h"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
MMRServer::MMRServer(QObject *parent) : QObject(parent) {
    initializeWsServer();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRServer::slotWsServerNewConnection() {
    QWebSocket *socket = wsServer->nextPendingConnection();

    QObject::connect(socket, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(slotWsBinaryMessageReceived(QByteArray)));
    QObject::connect(socket, SIGNAL(disconnected()), this, SLOT(slotWsDisconnected()));

    MMRConnection *connection = new MMRConnection(this);
    connection->ws = socket;

    wsMap.insert(socket, connection);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRServer::slotWsBinaryMessageReceived(QByteArray message) {
    QWebSocket *socket = qobject_cast<QWebSocket *>(sender());

    MMRConnection *connection = wsMap.value(socket, NULL);
    if (!connection) return;

    MMRWSData *wsData = new MMRWSData();
    wsData->loadFromByteArray(message);

    if (wsData->dataType == "request") {
        connection->handleRequest(wsData);
    }
}
//---------------------------------------------------------------------------
void MMRServer::slotWsDisconnected() {
    QWebSocket *socket = qobject_cast<QWebSocket *>(sender());

    MMRConnection *connection = wsMap.value(socket, NULL);
    if (connection) {
        connection->deleteLater();
    }

    wsMap.remove(socket);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRServer::initializeWsServer() {
    wsServer = new QWebSocketServer("MMRServer", QWebSocketServer::NonSecureMode, this);

    QObject::connect(wsServer, SIGNAL(newConnection()), this, SLOT(slotWsServerNewConnection()));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRServer::log(QString text) {
    IRQMSignalHandler::sendSignal("main", "log", text);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRServer::startServer(int port) {
    if (wsServer->isListening()) return;

    bool res = wsServer->listen(QHostAddress::Any, port);
    if (res) { // success
        this->log(QString("ws: Listening (%1)").arg(port));
        IRQMSignalHandler::sendSignal("mmrserver", "listening");
    }
    else {
        this->log(QString("ws: Failed to start listening (%1)").arg(port));
        IRQMSignalHandler::sendSignal("mmrserver", "listeningFailed");
    }
}
//---------------------------------------------------------------------------
void MMRServer::stopServer() {
    if (!wsServer->isListening()) return;

    wsServer->close();

    this->log("ws: Stopped");
    IRQMSignalHandler::sendSignal("mmrserver", "stopped");
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRServer::requestPrepareModalities() {
    if (!wsServer->isListening()) return;

    MMRWSData *data = new MMRWSData();
    data->requestType = "prepare";
    data->dataType = "request";

    sendRequest(data);

    data->deleteLater();
}
//---------------------------------------------------------------------------
void MMRServer::requestStartModalities() {
    if (!wsServer->isListening()) return;

    MMRWSData *data = new MMRWSData();
    data->requestType = "start";
    data->dataType = "request";

    sendRequest(data);

    data->deleteLater();
}
//---------------------------------------------------------------------------
void MMRServer::requestStopModalities() {
    if (!wsServer->isListening()) return;

    MMRWSData *data = new MMRWSData();
    data->requestType = "stop";
    data->dataType = "request";

    sendRequest(data);

    data->deleteLater();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRServer::sendRequest(MMRWSData *data) {
    QByteArray message = data->toByteArray();
    foreach (QWebSocket *socket, wsMap.keys()) {
        socket->sendBinaryMessage(message);
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
