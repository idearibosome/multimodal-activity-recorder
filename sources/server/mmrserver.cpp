#include "mmrserver.h"

#include "mmrmodalityconnection.h"
#include "mmrrecognizerconnection.h"

#include "../shared/irqm/irqmsignalhandler.h"

#include "../shared/mmrwsdata.h"
#include "../shared/mmrfilemetadata.h"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
MMRServer::MMRServer(QObject *parent) : QObject(parent) {
    initializeWsModalityServer();
    initializeWsRecognizerServer();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRServer::slotWsModalityServerNewConnection() {
    QWebSocket *socket = wsModalityServer->nextPendingConnection();

    MMRModalityConnection *connection = new MMRModalityConnection();
    connection->storageBasePath = storageBasePath;
    connection->setWebSocket(socket);

    QThread *connectionThread = new QThread(this);
    connection->moveToThread(connectionThread);

    QObject::connect(this, SIGNAL(modalityPrepare(MMRFileMetadata*)), connection, SLOT(slotPrepare(MMRFileMetadata*)));
    QObject::connect(this, SIGNAL(modalityStart()), connection, SLOT(slotStart()));
    QObject::connect(this, SIGNAL(modalityStop()), connection, SLOT(slotStop()));
    QObject::connect(this, SIGNAL(modalityFinalize()), connection, SLOT(slotFinalize()));

    QObject::connect(connection, SIGNAL(sendBinaryMessage(QWebSocket*,QByteArray)), this, SLOT(slotModalityConnectionSendBinaryMessage(QWebSocket*,QByteArray)));
    QObject::connect(connection, SIGNAL(closeWebSocket(QWebSocket*)), this, SLOT(slotModalityConnectionCloseWebSocket(QWebSocket*)));

    QObject::connect(connection, SIGNAL(disconnected()), this, SLOT(slotModalityConnectionDisconnected()));
    QObject::connect(connection, SIGNAL(disconnected()), connectionThread, SLOT(quit()));
    QObject::connect(connectionThread, SIGNAL(finished()), connectionThread, SLOT(deleteLater()));

    modalityConnections.append(connection);

    connectionThread->start();
}
//---------------------------------------------------------------------------
void MMRServer::slotWsRecognizerServerNewConnection() {
    QWebSocket *socket = wsRecognizerServer->nextPendingConnection();

    MMRRecognizerConnection *connection = new MMRRecognizerConnection();
    connection->setWebSocket(socket);

    QThread *connectionThread = new QThread(this);
    connection->moveToThread(connectionThread);

    QObject::connect(this, SIGNAL(recognizerPrepare()), connection, SLOT(slotPrepare()));
    QObject::connect(this, SIGNAL(recognizerStart()), connection, SLOT(slotStart()));
    QObject::connect(this, SIGNAL(recognizerStop()), connection, SLOT(slotStop()));
    QObject::connect(this, SIGNAL(recognizerFinalize()), connection, SLOT(slotFinalize()));

    QObject::connect(connection, SIGNAL(sendBinaryMessage(QWebSocket*,QByteArray)), this, SLOT(slotRecognizerConnectionSendBinaryMessage(QWebSocket*,QByteArray)));
    QObject::connect(connection, SIGNAL(closeWebSocket(QWebSocket*)), this, SLOT(slotRecognizerConnectionCloseWebSocket(QWebSocket*)));

    QObject::connect(connection, SIGNAL(disconnected()), this, SLOT(slotRecognizerConnectionDisconnected()));
    QObject::connect(connection, SIGNAL(disconnected()), connectionThread, SLOT(quit()));
    QObject::connect(connectionThread, SIGNAL(finished()), connectionThread, SLOT(deleteLater()));

    recognizerConnections.append(connection);

    connectionThread->start();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRServer::slotModalityConnectionDisconnected() {
    MMRModalityConnection *connection = qobject_cast<MMRModalityConnection *>(sender());
    connection->deleteLater();

    modalityConnections.removeAll(connection);
}
//---------------------------------------------------------------------------
void MMRServer::slotRecognizerConnectionDisconnected() {
    MMRRecognizerConnection *connection = qobject_cast<MMRRecognizerConnection *>(sender());
    connection->deleteLater();

    recognizerConnections.removeAll(connection);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRServer::slotModalityConnectionSendBinaryMessage(QWebSocket *ws, QByteArray message) {
    ws->sendBinaryMessage(message);
}
//---------------------------------------------------------------------------
void MMRServer::slotRecognizerConnectionSendBinaryMessage(QWebSocket *ws, QByteArray message) {
    ws->sendBinaryMessage(message);
}
//---------------------------------------------------------------------------
void MMRServer::slotModalityConnectionCloseWebSocket(QWebSocket *ws) {
    ws->close();
}
//---------------------------------------------------------------------------
void MMRServer::slotRecognizerConnectionCloseWebSocket(QWebSocket *ws) {
    ws->close();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRServer::initializeWsModalityServer() {
    wsModalityServer = new QWebSocketServer("MMRServer", QWebSocketServer::NonSecureMode, this);

    QObject::connect(wsModalityServer, SIGNAL(newConnection()), this, SLOT(slotWsModalityServerNewConnection()));
}
//---------------------------------------------------------------------------
void MMRServer::initializeWsRecognizerServer() {
    wsRecognizerServer = new QWebSocketServer("MMRServer_Recognizer", QWebSocketServer::NonSecureMode, this);

    QObject::connect(wsRecognizerServer, SIGNAL(newConnection()), this, SLOT(slotWsRecognizerServerNewConnection()));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRServer::log(QString text) {
    IRQMSignalHandler::sendSignal("main", "log", text);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRServer::startServer(int modalityPort, int recognizerPort) {
    if (!wsModalityServer->isListening() && modalityPort > 0) {
        bool res = wsModalityServer->listen(QHostAddress::Any, modalityPort);
        if (res) { // success
            this->log(QString("ws (modality): Listening (%1)").arg(modalityPort));
            IRQMSignalHandler::sendSignal("mmrserver", "listening");
        }
        else {
            this->log(QString("ws (modality): Failed to start listening (%1)").arg(modalityPort));
            IRQMSignalHandler::sendSignal("mmrserver", "listeningFailed");
        }
    }

    if (!wsRecognizerServer->isListening() && recognizerPort > 0) {
        bool res = wsRecognizerServer->listen(QHostAddress::Any, recognizerPort);
        if (res) { // success
            this->log(QString("ws (recognizer): Listening (%1)").arg(recognizerPort));
            IRQMSignalHandler::sendSignal("mmrserver", "listening");
        }
        else {
            this->log(QString("ws (recognizer): Failed to start listening (%1)").arg(recognizerPort));
            IRQMSignalHandler::sendSignal("mmrserver", "listeningFailed");
        }
    }
}
//---------------------------------------------------------------------------
void MMRServer::stopServer() {
    if (wsModalityServer->isListening()) {
        wsModalityServer->close();

        QList<MMRModalityConnection *> connections = this->modalityConnections;
        for (auto connection=modalityConnections.begin(); connection!=modalityConnections.end(); ++connection) {
            (*connection)->close();
        }

        this->log("ws (modality): Stopped");
    }
    if (wsRecognizerServer->isListening()) {
        wsRecognizerServer->close();

        // TODO

        this->log("ws (recognizer): Stopped");
    }

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

    emit modalityPrepare(fileMetadata);
    emit recognizerPrepare();
}
//---------------------------------------------------------------------------
void MMRServer::startModalityAcquisition() {
    this->log("ws: Start data acquisition");
    emit modalityStart();
    emit recognizerStart();
}
//---------------------------------------------------------------------------
void MMRServer::stopModalityAcquisition() {
    this->log("ws: Stop data acquisition");
    emit modalityStop();
    emit recognizerStop();
}
//---------------------------------------------------------------------------
void MMRServer::finalizeModalities() {
    this->log("ws: Finalize modalities");
    emit modalityFinalize();
    emit recognizerFinalize();

    finalizeFileMetadata();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRServer::sendModalityRequest(MMRWSData *data) {
    for (auto connection=modalityConnections.begin(); connection!=modalityConnections.end(); ++connection) {
        (*connection)->sendRequest(data);
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
MMRModalityConnection *MMRServer::modalityConnectionForIdentifier(QString identifier) {
    for (auto connection=modalityConnections.begin(); connection!=modalityConnections.end(); ++connection) {
        if ((*connection)->identifier == identifier) return (*connection);
    }
    return nullptr;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRServer::prepareFileMetadata() {
    if (fileMetadata) return;
    if (storageBasePath.isEmpty()) return;

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
