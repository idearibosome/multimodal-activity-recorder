#include "mmrconnection.h"

#include "../shared/irqm/irqmsignalhandler.h"

#include "../shared/mmrwsdata.h"
#include "../shared/mmrfilemetadata.h"
#include "../shared/mmrfiledata.h"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
MMRConnection::MMRConnection(QObject *parent) : QObject(parent) {
    identifier = "unknown";
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
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRConnection::handleRequest(MMRWSData *wsData) {
    if (!ws) return;

    QString type = wsData->requestType;
    QVariantMap data = wsData->data;

    if (type == "register") handleRequestRegister(type, data);
    else if (type == "data") handleRequestData(type, data);

    wsData->deleteLater();
}
//---------------------------------------------------------------------------
void MMRConnection::handleRequestRegister(QString type, QVariantMap data) {
    this->type = data.value("type").toString();
    this->identifier = data.value("identifier").toString();

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

    /*
    if (this->type == "qtsensor") {
        QByteArray sensorData = data.value("data").toByteArray();

        QDataStream inStream(sensorData);
        inStream.setVersion(QDataStream::Qt_5_9);

        qint64 timestamp;
        inStream >> timestamp;

        QString sensorType;
        inStream >> sensorType;

        if (sensorType == "accelerometer" || sensorType == "gyroscope") {
            qreal x, y, z;
            inStream >> x >> y >> z;
            qDebug() << timestamp << sensorType << x << y << z;
        }
        else if (sensorType == "lightsensor") {
            qreal lux;
            inStream >> lux;
            qDebug() << timestamp << sensorType << lux;
        }
        else if (sensorType == "magnetometer") {
            qreal cal, x, y, z;
            inStream >> cal >> x >> y >> z;
            qDebug() << timestamp << sensorType << cal << x << y << z;
        }

    }
    */
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
