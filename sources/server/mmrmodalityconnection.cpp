#include "mmrmodalityconnection.h"

#include "../shared/irqm/irqmsignalhandler.h"

#include "../shared/mmrwsdata.h"
#include "../shared/mmrfilemetadata.h"
#include "../shared/mmrfiledata.h"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
MMRModalityConnection::MMRModalityConnection(QObject *parent) : QObject(parent) {
    type = "";
    identifier = "";
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRModalityConnection::slotWsBinaryMessageReceived(QByteArray message) {
    MMRWSData *wsData = new MMRWSData();
    wsData->loadFromByteArray(message);

    if (wsData->dataType == "request") {
        handleRequest(wsData);
    }
}
//---------------------------------------------------------------------------
void MMRModalityConnection::slotWsDisconnected() {
    if (this->type != "") {
        log("disconnected (" + this->type + ", " + this->identifier + ")");
        IRQMSignalHandler::sendSignal("mmrconnection", "modalityDisconnected", this->type, this->identifier);
    }

    finalize();

    emit disconnected();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRModalityConnection::slotPrepare(MMRFileMetadata *fileMetadata) {
    setFileMetadata(fileMetadata);
    prepare();
}
//---------------------------------------------------------------------------
void MMRModalityConnection::slotStart() {
    start();
}
//---------------------------------------------------------------------------
void MMRModalityConnection::slotStop() {
    stop();
}
//---------------------------------------------------------------------------
void MMRModalityConnection::slotFinalize() {
    finalize();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRModalityConnection::log(QString text) {
    IRQMSignalHandler::sendSignal("main", "log", "[" + identifier.left(6) + "] " + text);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRModalityConnection::setWebSocket(QWebSocket *ws) {
    this->ws = ws;

    QObject::connect(ws, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(slotWsBinaryMessageReceived(QByteArray)), Qt::QueuedConnection);
    QObject::connect(ws, SIGNAL(disconnected()), this, SLOT(slotWsDisconnected()), Qt::QueuedConnection);
}
//---------------------------------------------------------------------------
void MMRModalityConnection::setFileMetadata(MMRFileMetadata *fileMetadata) {
    this->fileMetadata = fileMetadata;

    fileMetadata->addModality(type, identifier, QVariantMap());
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRModalityConnection::prepare() {
    prepareFile();

    MMRWSData *data = new MMRWSData();
    data->requestType = "prepare";
    data->dataType = "request";

    sendRequest(data);

    data->deleteLater();
}
//---------------------------------------------------------------------------
void MMRModalityConnection::start() {
    MMRWSData *data = new MMRWSData();
    data->requestType = "start";
    data->dataType = "request";

    sendRequest(data);

    data->deleteLater();
}
//---------------------------------------------------------------------------
void MMRModalityConnection::stop() {
    MMRWSData *data = new MMRWSData();
    data->requestType = "stop";
    data->dataType = "request";

    sendRequest(data);

    data->deleteLater();
}
//---------------------------------------------------------------------------
void MMRModalityConnection::finalize() {
    closeFile();

    MMRWSData *data = new MMRWSData();
    data->requestType = "finalize";
    data->dataType = "request";

    sendRequest(data);

    data->deleteLater();

    type = "";
    identifier = "";
}
//---------------------------------------------------------------------------
void MMRModalityConnection::close() {
    if (this->ws) {
        emit closeWebSocket(this->ws);
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRModalityConnection::handleRequest(MMRWSData *wsData) {
    if (!ws) return;

    QString type = wsData->requestType;
    QVariantMap data = wsData->data;

    if (type == "register") handleRequestRegister(type, data);
    else if (type == "data") handleRequestData(type, data);
    else if (type == "data_list") handleRequestDataList(type, data);

    wsData->deleteLater();
}
//---------------------------------------------------------------------------
void MMRModalityConnection::handleRequestRegister(QString type, QVariantMap data) {
    this->type = data.value("type").toString();
    this->identifier = data.value("identifier").toString();

    log("registered (" + this->type + ", " + this->identifier + ")");
    IRQMSignalHandler::sendSignal("mmrconnection", "modalityRegistered", this->type, this->identifier);

    MMRWSData wsData;
    wsData.requestType = type;
    wsData.dataType = "response";
    wsData.data.insert("result", QString("ok"));

    emit sendBinaryMessage(ws, wsData.toByteArray());
}
//---------------------------------------------------------------------------
void MMRModalityConnection::handleRequestData(QString type, QVariantMap data) {
    qint64 timestamp = data.value("timestamp").toULongLong();
    QByteArray dataByteArray = data.value("data").toByteArray();

    if (fileData && fileMetadata) {
        qint64 filePos = fileData->getCurrentFilePos();
        fileData->writeData(timestamp, dataByteArray);
        fileMetadata->addRecording(identifier, filePos, timestamp);
    }

    IRQMSignalHandler::sendSignal("mmrconnection", "receivedData", identifier);

    MMRWSData wsData;
    wsData.requestType = type;
    wsData.dataType = "response";
    wsData.data.insert("result", QString("ok"));

    emit sendBinaryMessage(ws, wsData.toByteArray());
}
//---------------------------------------------------------------------------
void MMRModalityConnection::handleRequestDataList(QString type, QVariantMap data) {
    if (fileData && fileMetadata) {
        fileMetadata->beginTransaction();

        QVariantList dataList = data.value("list").toList();
        qint64 totalDataSize = 0;
        for (int i=0; i<dataList.count(); i++) {
            QVariantMap eachData = dataList[i].toMap();

            qint64 timestamp = eachData.value("timestamp").toULongLong();
            QByteArray dataByteArray = eachData.value("data").toByteArray();

            qint64 filePos = fileData->getCurrentFilePos();
            fileData->writeData(timestamp, dataByteArray);
            fileMetadata->addRecording(identifier, filePos, timestamp);

            totalDataSize += dataByteArray.size();
        }

        fileMetadata->commitTransaction();
    }

    IRQMSignalHandler::sendSignal("mmrconnection", "receivedData", identifier);

    MMRWSData wsData;
    wsData.requestType = type;
    wsData.dataType = "response";
    wsData.data.insert("result", QString("ok"));

    emit sendBinaryMessage(ws, wsData.toByteArray());
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRModalityConnection::sendRequest(MMRWSData *wsData) {
    if (!ws) return;

    QByteArray message = wsData->toByteArray();
    emit sendBinaryMessage(ws, message);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRModalityConnection::prepareFile() {
    if (fileData) return;
    if (storageBasePath.isEmpty()) return;

    QString filePath = storageBasePath + QDir::separator() + type + "_" + identifier + ".mmr";
    filePath = QDir::cleanPath(filePath);

    fileData = new MMRFileData(this);

    fileData->setHeaderInfo("type", type);
    fileData->setHeaderInfo("identifier", identifier);

    fileData->prepareWritingToFilePath(filePath);
}
//---------------------------------------------------------------------------
void MMRModalityConnection::closeFile() {
    if (!fileData) return;

    fileData->finalizeWriting();
    fileData->close();

    fileData->deleteLater();
    fileData = NULL;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
