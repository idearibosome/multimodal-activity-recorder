#include "mmrconnection.h"

#include "../shared/irqm/irqmsignalhandler.h"

#include "../shared/mmrwsdata.h"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
MMRConnection::MMRConnection(QObject *parent) : QObject(parent) {
    identifier = "unknown";
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRConnection::log(QString text) {
    IRQMSignalHandler::sendSignal("main", "log", "[" + identifier.left(6) + "] " + text);
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

    openFile();

    MMRWSData wsData;
    wsData.requestType = type;
    wsData.dataType = "response";
    wsData.data.insert("result", QString("ok"));

    ws->sendBinaryMessage(wsData.toByteArray());
}
//---------------------------------------------------------------------------
void MMRConnection::handleRequestData(QString type, QVariantMap data) {
    if (!fileDataStream) return;

    QByteArray dataByteArray = data.value("data").toByteArray();

    *fileDataStream << dataByteArray;

    IRQMSignalHandler::sendSignal("mmrconnection", "receivedData", identifier, dataByteArray.size());
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MMRConnection::openFile() {
    if (file || fileDataStream) return;

    QString filePath = storageBasePath + QDir::separator() + type + "_" + identifier + ".mmr";
    filePath = QDir::cleanPath(filePath);

    file = new QFile(filePath, this);
    if (!file->open(QIODevice::WriteOnly)) {
        this->log("Failed to open file output device");

        file->deleteLater();
        file = NULL;
        return;
    }

    fileDataStream = new QDataStream(file);
    fileDataStream->setVersion(QDataStream::Qt_5_9);
}
//---------------------------------------------------------------------------
void MMRConnection::closeFile() {
    if (!fileDataStream) return;

    delete fileDataStream;
    fileDataStream = NULL;

    if (!file) return;

    file->close();
    file->deleteLater();
    file = NULL;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
