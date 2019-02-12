#include "mmrconnection.h"

#include "../shared/irqm/irqmsignalhandler.h"

#include "../shared/mmrwsdata.h"
#include "../shared/mmrfilemetadata.h"
#include "../shared/mmrfiledata.h"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
MMRConnection::MMRConnection(QObject *parent) : QObject(parent) {
    type = "";
    identifier = "";
}
//---------------------------------------------------------------------------
//--------------------------------------------------------------------------
void MMRConnection::slotWsBinaryMessageReceived(QByteArray message) {
    MMRWSData *wsData = new MMRWSData();
    wsData->loadFromByteArray(message);

    if (wsData->dataType == "request") {
        handleRequest(wsData);
    }
}
//---------------------------------------------------------------------------
void MMRConnection::slotWsDisconnected() {
    if (this->type != "") {
        log("disconnected (" + this->type + ", " + this->identifier + ")");
        IRQMSignalHandler::sendSignal("mmrconnection", "disconnected", this->type, this->identifier);
    }

    finalize();

    emit disconnected();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRConnection::log(QString text) {
    IRQMSignalHandler::sendSignal("main", "log", "[" + identifier.left(6) + "] " + text);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRConnection::setWebSocket(QWebSocket *ws) {
    this->ws = ws;

    QObject::connect(ws, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(slotWsBinaryMessageReceived(QByteArray)));
    QObject::connect(ws, SIGNAL(disconnected()), this, SLOT(slotWsDisconnected()));
}
//---------------------------------------------------------------------------
void MMRConnection::setFileMetadata(MMRFileMetadata *fileMetadata) {
    this->fileMetadata = fileMetadata;

    fileMetadata->addModality(type, identifier, QVariantMap());
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRConnection::prepare() {
    prepareFile();

    MMRWSData *data = new MMRWSData();
    data->requestType = "prepare";
    data->dataType = "request";

    sendRequest(data);

    data->deleteLater();
}
//---------------------------------------------------------------------------
void MMRConnection::start() {
    MMRWSData *data = new MMRWSData();
    data->requestType = "start";
    data->dataType = "request";

    sendRequest(data);

    data->deleteLater();
}
//---------------------------------------------------------------------------
void MMRConnection::stop() {
    MMRWSData *data = new MMRWSData();
    data->requestType = "stop";
    data->dataType = "request";

    sendRequest(data);

    data->deleteLater();
}
//---------------------------------------------------------------------------
void MMRConnection::finalize() {
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
void MMRConnection::close() {
    if (this->ws) {
        this->ws->close();
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRConnection::handleRequest(MMRWSData *wsData) {
    if (!ws) return;

    QString type = wsData->requestType;
    QVariantMap data = wsData->data;

    if (type == "register") handleRequestRegister(type, data);
    else if (type == "data") handleRequestData(type, data);
    else if (type == "data_list") handleRequestDataList(type, data);

    wsData->deleteLater();
}
//---------------------------------------------------------------------------
void MMRConnection::handleRequestRegister(QString type, QVariantMap data) {
    this->type = data.value("type").toString();
    this->identifier = data.value("identifier").toString();

    log("registered (" + this->type + ", " + this->identifier + ")");
    IRQMSignalHandler::sendSignal("mmrconnection", "registered", this->type, this->identifier);

    MMRWSData wsData;
    wsData.requestType = type;
    wsData.dataType = "response";
    wsData.data.insert("result", QString("ok"));

    ws->sendBinaryMessage(wsData.toByteArray());
}
//---------------------------------------------------------------------------
void MMRConnection::handleRequestData(QString type, QVariantMap data) {
    if (!fileData) return;

    qint64 timestamp = data.value("timestamp").toULongLong();
    QByteArray dataByteArray = data.value("data").toByteArray();

    qint64 filePos = fileData->getCurrentFilePos();
    fileData->writeData(timestamp, dataByteArray);
    fileMetadata->addRecording(identifier, filePos, timestamp);

    IRQMSignalHandler::sendSignal("mmrconnection", "receivedData", identifier, dataByteArray.size());

    MMRWSData wsData;
    wsData.requestType = type;
    wsData.dataType = "response";
    wsData.data.insert("result", QString("ok"));

    ws->sendBinaryMessage(wsData.toByteArray());
}
//---------------------------------------------------------------------------
void MMRConnection::handleRequestDataList(QString type, QVariantMap data) {
    if (!fileData) return;

    QVariantList dataList = data.value("list").toList();
    qint64 totalDataSize = 0;
    for (int i=0; i<dataList.count(); i++) {
        QVariantMap eachData = dataList[i].toMap();

        qint64 timestamp = data.value("timestamp").toULongLong();
        QByteArray dataByteArray = data.value("data").toByteArray();

        qint64 filePos = fileData->getCurrentFilePos();
        fileData->writeData(timestamp, dataByteArray);
        fileMetadata->addRecording(identifier, filePos, timestamp);

        totalDataSize += dataByteArray.size();
    }

    IRQMSignalHandler::sendSignal("mmrconnection", "receivedData", identifier, totalDataSize);

    MMRWSData wsData;
    wsData.requestType = type;
    wsData.dataType = "response";
    wsData.data.insert("result", QString("ok"));

    ws->sendBinaryMessage(wsData.toByteArray());
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRConnection::sendRequest(MMRWSData *wsData) {
    if (!ws) return;

    QByteArray message = wsData->toByteArray();
    ws->sendBinaryMessage(message);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRConnection::prepareFile() {
    if (fileData) return;

    QString filePath = storageBasePath + QDir::separator() + type + "_" + identifier + ".mmr";
    filePath = QDir::cleanPath(filePath);

    fileData = new MMRFileData(this);

    fileData->setHeaderInfo("type", type);
    fileData->setHeaderInfo("identifier", identifier);

    fileData->prepareWritingToFilePath(filePath);
}
//---------------------------------------------------------------------------
void MMRConnection::closeFile() {
    if (!fileData) return;

    fileData->finalizeWriting();
    fileData->close();

    fileData->deleteLater();
    fileData = NULL;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
