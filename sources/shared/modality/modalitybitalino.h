#ifndef MODALITYBITALINO_H
#define MODALITYBITALINO_H

#if defined(MMR_MODALITY_BITALINO)

#include <QBluetoothDeviceDiscoveryAgent>
#include <QDataStream>
#include <QLowEnergyController>
#include <QObject>

#include "modality.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class ModalityBITalinoFrame : public QObject
{
    Q_OBJECT
public:
    ModalityBITalinoFrame(QObject *parent = nullptr);

    bool isValid;

    char crc;
    char sequenceNumber;
    bool digitalInputStates[2];
    bool digitalOutputStates[2];
    int numChannels;
    qint16 analogData[6];

    static ModalityBITalinoFrame *fromByteArray(const QByteArray &byteArray, int numChannels);

    QString toString();
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class ModalityBITalino : public Modality
{
    Q_OBJECT
public:
    explicit ModalityBITalino(QObject *parent = nullptr);

    int samplingRate;

    bool analogChannelAvailabilities[6];
    int numAvailableAnalogChannels;

    QBluetoothDeviceDiscoveryAgent *btDeviceDiscoveryAgent = 0;
    QLowEnergyController *btLEController = 0;

    QVariantList btDiscoveredDeviceList;
    QList<QBluetoothDeviceInfo> btDiscoveredDeviceInfoList;

    QLowEnergyService *btExchangeService = 0;

    bool initialize(QVariantMap configuration) override;
    void reset() override;

    bool startAcquisition() override;
    void stopAcquisition() override;

    QVariantList parseData(QByteArray data, EModalityParseType parseType) override;

    Q_INVOKABLE void startDiscovery();
    Q_INVOKABLE void stopDiscovery();

    Q_INVOKABLE void setSamplingRate(int rate);
    Q_INVOKABLE void setAnalogChannelAvailability(int channel, bool availability);

    Q_INVOKABLE void connectToDevice(int index);
    Q_INVOKABLE void disconnectFromDevice();

signals:
    void deviceDiscovered(QVariantList deviceList);

    void deviceConnected();
    void deviceConnectionFailed(QString code);

private slots:
    void slotBtDeviceDiscoveryFinished();

    void slotBtServiceCharacteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue);
    void slotBtServiceCharacteristicWritten(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue);
    void slotBtServiceStateChanged(QLowEnergyService::ServiceState newState);
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#endif // MMR_MODALITY_BITALINO

#endif // MODALITYBITALINO_H
