#include "mmrserver.h"

#include "mmrconnection.h"

#include "../shared/irqm/irqmsignalhandler.h"

#include "../shared/mmrwsdata.h"
#include "../shared/mmrfilemetadata.h"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
MMRServer::MMRServer(QObject *parent) : QObject(parent) {
    initializeWsServer();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRServer::slotWsServerNewConnection() {
    QWebSocket *socket = wsServer->nextPendingConnection();

    MMRConnection *connection = new MMRConnection();
    connection->storageBasePath = storageBasePath;
    connection->setWebSocket(socket);

    QThread *connectionThread = new QThread(this);
    connection->moveToThread(connectionThread);

    QObject::connect(this, SIGNAL(prepare(MMRFileMetadata*)), connection, SLOT(slotPrepare(MMRFileMetadata*)));
    QObject::connect(this, SIGNAL(start()), connection, SLOT(slotStart()));
    QObject::connect(this, SIGNAL(stop()), connection, SLOT(slotStop()));
    QObject::connect(this, SIGNAL(finalize()), connection, SLOT(slotFinalize()));

    QObject::connect(connection, SIGNAL(sendBinaryMessage(QWebSocket*,QByteArray)), this, SLOT(slotConnectionSendBinaryMessage(QWebSocket*,QByteArray)));
    QObject::connect(connection, SIGNAL(closeWebSocket(QWebSocket*)), this, SLOT(slotConnectionCloseWebSocket(QWebSocket*)));

    QObject::connect(connection, SIGNAL(disconnected()), this, SLOT(slotConnectionDisconnected()));
    QObject::connect(connection, SIGNAL(disconnected()), connectionThread, SLOT(quit()));
    QObject::connect(connectionThread, SIGNAL(finished()), connectionThread, SLOT(deleteLater()));

    connections.append(connection);

    connectionThread->start();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRServer::slotConnectionDisconnected() {
    MMRConnection *connection = qobject_cast<MMRConnection *>(sender());
    connection->deleteLater();

    connections.removeAll(connection);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRServer::slotConnectionSendBinaryMessage(QWebSocket *ws, QByteArray message) {
    ws->sendBinaryMessage(message);
}
//---------------------------------------------------------------------------
void MMRServer::slotConnectionCloseWebSocket(QWebSocket *ws) {
    ws->close();
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

    QList<MMRConnection *> connections = this->connections;
    for (auto connection=connections.begin(); connection!=connections.end(); ++connection) {
        (*connection)->close();
    }

    this->log("ws: Stopped");
    IRQMSignalHandler::sendSignal("mmrserver", "stopped");
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRServer::setStorageBasePath(QString path) {
    this->log("Set storage base path: " + path);
    storageBasePath = path;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRServer::prepareModalities() {
    this->log("ws: Prepare modalities");
    prepareFileMetadata();

    emit prepare(fileMetadata);
}
//---------------------------------------------------------------------------
void MMRServer::startModalityAcquisition() {
    this->log("ws: Start data acquisition");
    emit start();
}
//---------------------------------------------------------------------------
void MMRServer::stopModalityAcquisition() {
    this->log("ws: Stop data acquisition");
    emit stop();
}
//---------------------------------------------------------------------------
void MMRServer::finalizeModalities() {
    this->log("ws: Finalize modalities");
    emit finalize();

    finalizeFileMetadata();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRServer::sendRequest(MMRWSData *data) {
    for (auto connection=connections.begin(); connection!=connections.end(); ++connection) {
        (*connection)->sendRequest(data);
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRServer::prepareFileMetadata() {
    if (fileMetadata) return;

    fileMetadata = new MMRFileMetadata(this);

    fileMetadata->createToFileDirPath(storageBasePath, true);
}
//---------------------------------------------------------------------------
void MMRServer::finalizeFileMetadata() {
    if (!fileMetadata) return;

    fileMetadata->finalizeWriting();
    fileMetadata->close();

    fileMetadata->deleteLater();
    fileMetadata = NULL;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
