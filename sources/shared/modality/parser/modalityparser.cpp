#include "modalityparser.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ModalityParser::ModalityParser(QObject *parent) : QObject(parent) {
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
QVariant ModalityParser::parsedDataItemWithValue(QString name, QVariant value) {
    QVariantMap itemMap;

    itemMap.insert("name", name);
    itemMap.insert("type", "value");
    itemMap.insert("value", value);

    return itemMap;
}
//---------------------------------------------------------------------------
QVariant ModalityParser::parsedDataItemWithByteArray(QString name, QByteArray byteArray) {
    QVariantMap itemMap;

    itemMap.insert("name", name);
    itemMap.insert("type", "byte_array");
    itemMap.insert("byte_array", byteArray);

    return itemMap;
}
//---------------------------------------------------------------------------
QVariant ModalityParser::parsedDataItemWithImage(QString name, QImage image) {
    QVariantMap itemMap;

    itemMap.insert("name", name);
    itemMap.insert("type", "image");
    itemMap.insert("image", image);

    return itemMap;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
