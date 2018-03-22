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
    QVariantMap sensorMap = configuration.value("sensor").toMap();
    if (sensorMap.isEmpty()) return false;

    sensorType = sensorMap.value("type").toString();
    if (sensorType.isEmpty()) return false;

    QByteArray sensorId = sensorMap.value("identifier").toByteArray();

    if (sensorType == "accelerometer") {
        sensor = new QAccelerometer(this);
    }
    else if (sensorType == "compass") {
        sensor = new QCompass(this);
    }
    else if (sensorType == "gyroscope") {
        sensor = new QGyroscope(this);
    }
    else if (sensorType == "lightsensor") {
        sensor = new QLightSensor(this);
    }
    else if (sensorType == "magnetometer") {
        sensor = new QMagnetometer(this);
    }
    else {
        return false;
    }

    sensor->setIdentifier(sensorId);
    sensor->setAlwaysOn(true);

    QObject::connect(sensor, SIGNAL(readingChanged()), this, SLOT(slotSensorReadingChanged()));

    return Modality::initialize(configuration);
}
//---------------------------------------------------------------------------
void ModalityQtSensor::reset() {
    if (sensor) {
        sensor->deleteLater();
        sensor = NULL;
    }

    Modality::reset();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void ModalityQtSensor::slotSensorReadingChanged() {
    qint64 timestamp = getAcquisitionTimestamp();

    QByteArray byteArray;
    QDataStream outStream(&byteArray, QIODevice::WriteOnly);
    outStream.setVersion(QDataStream::Qt_5_9);

    outStream << sensorType;

    bool isDataAcquired = false;

    if (sensorType == "accelerometer") {
        QAccelerometerReading *reading = ((QAccelerometer *)sensor)->reading();
        if (reading) {
            outStream << reading->x();
            outStream << reading->y();
            outStream << reading->z();
            isDataAcquired = true;
        }
    }
    else if (sensorType == "compass") {
        QCompassReading *reading = ((QCompass *)sensor)->reading();
        if (reading) {
            outStream << reading->calibrationLevel();
            outStream << reading->azimuth();
            isDataAcquired = true;
        }
    }
    else if (sensorType == "gyroscope") {
        QGyroscopeReading *reading = ((QGyroscope *)sensor)->reading();
        if (reading) {
            outStream << reading->x();
            outStream << reading->y();
            outStream << reading->z();
            isDataAcquired = true;
        }
    }
    else if (sensorType == "lightsensor") {
        QLightReading *reading = ((QLightSensor *)sensor)->reading();
        if (reading) {
            outStream << reading->lux();
            isDataAcquired = true;
        }
    }
    else if (sensorType == "magnetometer") {
        QMagnetometerReading *reading = ((QMagnetometer *)sensor)->reading();
        if (reading) {
            outStream << reading->calibrationLevel();
            outStream << reading->x();
            outStream << reading->y();
            outStream << reading->z();
            isDataAcquired = true;
        }
    }

    if (isDataAcquired) {
        emit acquired(timestamp, byteArray);
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool ModalityQtSensor::startAcquisition() {
    if (!sensor) return false;

    startRecordingAcquisitionTimestamp();
    sensor->setActive(true);

    return true;
}
//---------------------------------------------------------------------------
void ModalityQtSensor::stopAcquisition() {
    sensor->setActive(false);
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
