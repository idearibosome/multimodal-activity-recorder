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
