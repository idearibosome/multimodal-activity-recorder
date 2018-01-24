#if defined(MMR_MODALITY_QTSENSOR)

#include "modalityqtsensor.h"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ModalityQtSensor::ModalityQtSensor(QObject *parent) : Modality(parent) {
    type = "qtsensor";
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool ModalityQtSensor::initialize(QVariantMap configuration) {
    return Modality::initialize(configuration);
}
//---------------------------------------------------------------------------
void ModalityQtSensor::reset() {
    Modality::reset();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void ModalityQtSensor::slotSensorReadingChanged() {

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool ModalityQtSensor::startAcquisition() {
    return false;
}
//---------------------------------------------------------------------------
void ModalityQtSensor::stopAcquisition() {
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
QVariantList ModalityQtSensor::getAvailableSensors() {
    QVariantList sensorList;
    QList<QByteArray> sensorIdList;

    sensorIdList = QSensor::sensorsForType(QAccelerometer::type);
    foreach (QByteArray sensorId, sensorIdList) {
        QVariantMap sensorMap;
        sensorMap.insert("type", "accelerometer");
        sensorMap.insert("text", "Accelerometer");
        sensorMap.insert("identifier", sensorId);

        sensorList.append(sensorMap);
    }

    sensorIdList = QSensor::sensorsForType(QCompass::type);
    foreach (QByteArray sensorId, sensorIdList) {
        QVariantMap sensorMap;
        sensorMap.insert("type", "compass");
        sensorMap.insert("text", "Compass");
        sensorMap.insert("identifier", sensorId);

        sensorList.append(sensorMap);
    }

    sensorIdList = QSensor::sensorsForType(QGyroscope::type);
    foreach (QByteArray sensorId, sensorIdList) {
        QVariantMap sensorMap;
        sensorMap.insert("type", "gyroscope");
        sensorMap.insert("text", "Gyroscope");
        sensorMap.insert("identifier", sensorId);

        sensorList.append(sensorMap);
    }

    sensorIdList = QSensor::sensorsForType(QLightSensor::type);
    foreach (QByteArray sensorId, sensorIdList) {
        QVariantMap sensorMap;
        sensorMap.insert("type", "lightsensor");
        sensorMap.insert("text", "Light");
        sensorMap.insert("identifier", sensorId);

        sensorList.append(sensorMap);
    }

    sensorIdList = QSensor::sensorsForType(QMagnetometer::type);
    foreach (QByteArray sensorId, sensorIdList) {
        QVariantMap sensorMap;
        sensorMap.insert("type", "magnetometer");
        sensorMap.insert("text", "Magnetometer");
        sensorMap.insert("identifier", sensorId);

        sensorList.append(sensorMap);
    }

    return sensorList;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#endif
