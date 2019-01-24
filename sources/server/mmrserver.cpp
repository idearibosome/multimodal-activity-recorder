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

    MMRConnection *connection = new MMRConnection(this);
    connection->storageBasePath = storageBasePath;

    QObject::connect(connection, SIGNAL(disconnected()), this, SLOT(slotConnectionDisconnected()));

    connection->setWebSocket(socket);

    connections.append(connection);
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

    for (auto connection=connections.begin(); connection!=connections.end(); ++connection) {
        (*connection)->setFileMetadata(fileMetadata);
        (*connection)->prepare();
    }
}
//---------------------------------------------------------------------------
void MMRServer::startModalityAcquisition() {
    this->log("ws: Start data acquisition");
    for (auto connection=connections.begin(); connection!=connections.end(); ++connection) {
        (*connection)->start();
    }
}
//---------------------------------------------------------------------------
void MMRServer::stopModalityAcquisition() {
    this->log("ws: Stop data acquisition");
    for (auto connection=connections.begin(); connection!=connections.end(); ++connection) {
        (*connection)->stop();
    }
}
//---------------------------------------------------------------------------
void MMRServer::finalizeModalities() {
    this->log("ws: Finalize modalities");
    for (auto connection=connections.begin(); connection!=connections.end(); ++connection) {
        (*connection)->finalize();
    }

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

    fileMetadata->createToFileDirPath(storageBasePath);
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
