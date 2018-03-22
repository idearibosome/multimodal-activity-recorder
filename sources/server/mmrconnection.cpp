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

    qint64 timestamp = data.value("timestamp").toULongLong();
    QByteArray dataByteArray = data.value("data").toByteArray();

    *fileDataStream << timestamp << dataByteArray;

    IRQMSignalHandler::sendSignal("mmrconnection", "receivedData", identifier, dataByteArray.size());

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
