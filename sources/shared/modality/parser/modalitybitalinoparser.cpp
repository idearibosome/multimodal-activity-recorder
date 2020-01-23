#include "modalitybitalinoparser.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ModalityBITalinoParser::ModalityBITalinoParser(QObject *parent) : ModalityParser(parent) {
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
QVariantList ModalityBITalinoParser::parseData(QByteArray data, EModalityParserParseType parseType) {
    QVariantList parsedDataList;

    QDataStream inStream(&data, QIODevice::ReadOnly);
    inStream.setVersion(QDataStream::Qt_5_9);

    int numChannels;
    inStream >> numChannels;
    parsedDataList.append(ModalityParser::parsedDataItemWithValue("num_channels", numChannels));

    for (int i=0; i<numChannels; i++) {
        qint16 analogData;
        inStream >> analogData;
        parsedDataList.append(ModalityParser::parsedDataItemWithValue(QString("c%1").arg(i), analogData));
    }

    return parsedDataList;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
