#include "modalityfitbitparser.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ModalityFitbitParser::ModalityFitbitParser(QObject *parent) : ModalityParser(parent) {
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
QVariantList ModalityFitbitParser::parseData(QByteArray data, EModalityParserParseType parseType) {
    QVariantList parsedDataList;

    QDataStream inStream(&data, QIODevice::ReadOnly);
    inStream.setVersion(QDataStream::Qt_5_9);

    QVariantMap parsedData;
    inStream >> parsedData;

    if (parsedData.contains("accel")) {
        QVariantMap currentMap = parsedData.value("accel").toMap();
        parsedDataList.append(ModalityParser::parsedDataItemWithValue("acc_x", currentMap.value("x")));
        parsedDataList.append(ModalityParser::parsedDataItemWithValue("acc_y", currentMap.value("y")));
        parsedDataList.append(ModalityParser::parsedDataItemWithValue("acc_z", currentMap.value("z")));
    }
    if (parsedData.contains("bar")) {
        QVariantMap currentMap = parsedData.value("bar").toMap();
        parsedDataList.append(ModalityParser::parsedDataItemWithValue("pressure", currentMap.value("pressure")));
    }
    if (parsedData.contains("bps")) {
        QVariantMap currentMap = parsedData.value("bps").toMap();
        parsedDataList.append(ModalityParser::parsedDataItemWithValue("body_presence", currentMap.value("presence")));
    }
    if (parsedData.contains("gyro")) {
        QVariantMap currentMap = parsedData.value("gyro").toMap();
        parsedDataList.append(ModalityParser::parsedDataItemWithValue("gyro_x", currentMap.value("x")));
        parsedDataList.append(ModalityParser::parsedDataItemWithValue("gyro_y", currentMap.value("y")));
        parsedDataList.append(ModalityParser::parsedDataItemWithValue("gyro_z", currentMap.value("z")));
    }
    if (parsedData.contains("hrm")) {
        QVariantMap currentMap = parsedData.value("hrm").toMap();
        parsedDataList.append(ModalityParser::parsedDataItemWithValue("heart_rate", currentMap.value("heartRate")));
    }
    if (parsedData.contains("orientation")) {
        QVariantList currentList = parsedData.value("orientation").toList();
        parsedDataList.append(ModalityParser::parsedDataItemWithValue("ori_scalar", currentList[0]));
        parsedDataList.append(ModalityParser::parsedDataItemWithValue("ori_i", currentList[1]));
        parsedDataList.append(ModalityParser::parsedDataItemWithValue("ori_j", currentList[2]));
        parsedDataList.append(ModalityParser::parsedDataItemWithValue("ori_k", currentList[3]));
    }

    return parsedDataList;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
