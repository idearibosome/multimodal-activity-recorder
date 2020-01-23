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
