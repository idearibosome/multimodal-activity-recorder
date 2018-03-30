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
QVariantMap Modality::parsedDataItemWithValue(QString name, QVariant value) {
    QVariantMap itemMap;

    itemMap.insert("name", name);
    itemMap.insert("type", "value");
    itemMap.insert("value", value);

    return itemMap;
}
//---------------------------------------------------------------------------
QVariantMap Modality::parsedDataItemWithImage(QString name, QImage image) {
    QVariantMap itemMap;

    itemMap.insert("name", name);
    itemMap.insert("type", "image");
    itemMap.insert("image", image);

    return itemMap;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void Modality::startRecordingAcquisitionTimestamp() {
    this->startAcquisitionTimestamp = QDateTime::currentMSecsSinceEpoch();
}
//---------------------------------------------------------------------------
qint64 Modality::getAcquisitionTimestamp() {
    qint64 currentTimestamp = QDateTime::currentMSecsSinceEpoch();
    return currentTimestamp - this->startAcquisitionTimestamp;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
