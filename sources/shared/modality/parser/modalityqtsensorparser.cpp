#include "modalityqtsensorparser.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ModalityQtSensorParser::ModalityQtSensorParser(QObject *parent) : ModalityParser(parent) {
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
QVariantList ModalityQtSensorParser::parseData(QByteArray data, EModalityParserParseType parseType) {
    QVariantList parsedDataList;

    QDataStream inStream(&data, QIODevice::ReadOnly);
    inStream.setVersion(QDataStream::Qt_5_9);

    QString sensorType;
    inStream >> sensorType;
    parsedDataList.append(ModalityParser::parsedDataItemWithValue("type", sensorType));

    if (sensorType == "accelerometer") {
        qreal x, y, z;
        inStream >> x >> y >> z;
        parsedDataList.append(ModalityParser::parsedDataItemWithValue("x", x));
        parsedDataList.append(ModalityParser::parsedDataItemWithValue("y", y));
        parsedDataList.append(ModalityParser::parsedDataItemWithValue("z", z));
    }
    else if (sensorType == "ambient_temperature") {
        qreal temperature;
        inStream >> temperature;
        parsedDataList.append(ModalityParser::parsedDataItemWithValue("temperature", temperature));
    }
    else if (sensorType == "compass") {
        qreal azimuth, calibrationLevel;
        inStream >> azimuth >> calibrationLevel;
        parsedDataList.append(ModalityParser::parsedDataItemWithValue("azimuth", azimuth));
        parsedDataList.append(ModalityParser::parsedDataItemWithValue("calibration_level", calibrationLevel));
    }
    else if (sensorType == "gyroscope") {
        qreal x, y, z;
        inStream >> x >> y >> z;
        parsedDataList.append(ModalityParser::parsedDataItemWithValue("x", x));
        parsedDataList.append(ModalityParser::parsedDataItemWithValue("y", y));
        parsedDataList.append(ModalityParser::parsedDataItemWithValue("z", z));
    }
    else if (sensorType == "lightsensor") {
        qreal lux;
        inStream >> lux;
        parsedDataList.append(ModalityParser::parsedDataItemWithValue("lux", lux));
    }
    else if (sensorType == "magnetometer") {
        qreal calibrationLevel, x, y, z;
        inStream >> calibrationLevel >> x >> y >> z;
        parsedDataList.append(ModalityParser::parsedDataItemWithValue("calibration_level", calibrationLevel));
        parsedDataList.append(ModalityParser::parsedDataItemWithValue("x", x));
        parsedDataList.append(ModalityParser::parsedDataItemWithValue("y", y));
        parsedDataList.append(ModalityParser::parsedDataItemWithValue("z", z));
    }
    else if (sensorType == "pressure") {
        qreal pressure;
        inStream >> pressure;
        parsedDataList.append(ModalityParser::parsedDataItemWithValue("pressure", pressure));
    }
    else if (sensorType == "proximity") {
        bool close;
        inStream >> close;
        parsedDataList.append(ModalityParser::parsedDataItemWithValue("close", close));
    }

    return parsedDataList;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
