#include "modality.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
Modality::Modality(QObject *parent) : QObject(parent) {
    this->identifier = QUuid::createUuid().toString().replace(QRegularExpression("[^0-9A-Za-z]"), "");
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool Modality::initialize(QVariantMap configuration) {
    this->configuration = configuration;

    return true;
}
//---------------------------------------------------------------------------
void Modality::reset() {
    this->configuration = QVariantMap();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
QVariant Modality::parsedDataItemWithValue(QString name, QVariant value) {
    QVariantMap itemMap;

    itemMap.insert("name", name);
    itemMap.insert("type", "value");
    itemMap.insert("value", value);

    return itemMap;
}
//---------------------------------------------------------------------------
QVariant Modality::parsedDataItemWithByteArray(QString name, QByteArray byteArray) {
    QVariantMap itemMap;

    itemMap.insert("name", name);
    itemMap.insert("type", "byte_array");
    itemMap.insert("byte_array", byteArray);

    return itemMap;
}
//---------------------------------------------------------------------------
QVariant Modality::parsedDataItemWithImage(QString name, QImage image) {
    QVariantMap itemMap;

    itemMap.insert("name", name);
    itemMap.insert("type", "image");
    itemMap.insert("image", image);

    return itemMap;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
qint64 Modality::getTimestamp() {
    qint64 timestamp = QDateTime::currentMSecsSinceEpoch();
    return timestamp;
}
//---------------------------------------------------------------------------
void Modality::startRecordingAcquisitionTimestamp() {
    this->startAcquisitionTimestamp = getTimestamp();
}
//---------------------------------------------------------------------------
qint64 Modality::getAcquisitionTimestamp(qint64 timestamp) {
    if (timestamp == 0) timestamp = getTimestamp();
    return timestamp - this->startAcquisitionTimestamp;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
