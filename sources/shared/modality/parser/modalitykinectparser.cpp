#include "modalitykinectparser.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ModalityKinectParser::ModalityKinectParser(QObject *parent) : ModalityParser(parent) {
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
QVariantList ModalityKinectParser::parseData(QByteArray data, EModalityParserParseType parseType) {
    QVariantList parsedDataList;

    QDataStream inStream(&data, QIODevice::ReadOnly);
    inStream.setVersion(QDataStream::Qt_5_9);

    int colorFrameWidth, colorFrameHeight;
    QByteArray colorData;

    inStream >> colorFrameWidth >> colorFrameHeight >> colorData;

    if (parseType == ParseType_Recognizer) {
        parsedDataList.append(ModalityParser::parsedDataItemWithValue("color_width", colorFrameWidth));
        parsedDataList.append(ModalityParser::parsedDataItemWithValue("color_height", colorFrameHeight));
    }

    if (parseType == ParseType_Viewer) {
        QImage colorImage = QImage((const uchar *)colorData.constData(), colorFrameWidth, colorFrameHeight, QImage::Format_RGB888).copy();
        parsedDataList.append(ModalityParser::parsedDataItemWithImage("color", colorImage));
    }
    else {
        parsedDataList.append(ModalityParser::parsedDataItemWithByteArray("color", colorData));
    }

    int depthFrameWidth, depthFrameHeight;
    QByteArray depthData;

    inStream >> depthFrameWidth >> depthFrameHeight >> depthData;

    if (parseType == ParseType_Recognizer) {
        parsedDataList.append(ModalityParser::parsedDataItemWithValue("depth_width", depthFrameWidth));
        parsedDataList.append(ModalityParser::parsedDataItemWithValue("depth_height", depthFrameHeight));
    }

    if (parseType == ParseType_Viewer) {
        QImage depthImage = QImage((const uchar *)depthData.constData(), depthFrameWidth, depthFrameHeight, QImage::Format_RGB16).copy();
        parsedDataList.append(ModalityParser::parsedDataItemWithImage("depth", depthImage));
    }
    else {
        parsedDataList.append(ModalityParser::parsedDataItemWithByteArray("depth", depthData));
    }

    return parsedDataList;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
