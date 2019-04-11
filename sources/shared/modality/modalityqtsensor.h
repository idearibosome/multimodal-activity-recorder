#ifndef MODALITYQTSENSOR_H
#define MODALITYQTSENSOR_H

#if defined(MMR_MODALITY_QTSENSOR)

#include <QObject>
#include <QtSensors/QtSensors>

#include "modality.h"

class ModalityQtSensor : public Modality
{
    Q_OBJECT
public:
    explicit ModalityQtSensor(QObject *parent = nullptr);

    QSensor *sensor;
    QString sensorType;

    bool initialize(QVariantMap configuration) override;
    void reset() override;

    bool startAcquisition() override;
    void stopAcquisition() override;

    QVariantList parseData(QByteArray data, EModalityParseType parseType) override;

    Q_INVOKABLE static QVariantList getAvailableSensors();

private slots:
    void slotSensorReadingChanged();

};

#endif

#endif // MODALITYQTSENSOR_H
