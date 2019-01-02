#if defined(MMR_MODALITY_BITALINO)

/*
 * References:
 * http://bitalino.com/datasheets/REVOLUTION_MCU_Block_Datasheet.pdf
 * http://bitalino.com/datasheets/REVOLUTION_BLE_Block_Datasheet.pdf
 */

#define MMR_MODALITY_BITALINO_EXCHANGE_UUID QString("{c566488a-0882-4e1b-a6d0-0b717e652234}")
#define MMR_MODALITY_BITALINO_COMMANDS_UUID QString("{4051eb11-bf0a-4c74-8730-a48f4193fcea}")
#define MMR_MODALITY_BITALINO_FRAMES_UUID QString("{40fdba6b-672e-47c4-808a-e529adff3633}")

#include "modalitybitalino.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ModalityBITalinoFrame::ModalityBITalinoFrame(QObject *parent) : QObject(parent) {
    isValid = false;
    numChannels = 0;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ModalityBITalinoFrame *ModalityBITalinoFrame::fromByteArray(const QByteArray &byteArray, int numChannels) {
    ModalityBITalinoFrame *frame = new ModalityBITalinoFrame();

    if (byteArray.isEmpty()) return frame;


    int currentIndex = byteArray.length()-1;


    // crc
    frame->crc = (byteArray.at(currentIndex) & 0x0F);

    char x[4] = {0, 0, 0, 0};
    char out;
    for (int i=0; i<byteArray.length(); i++) {
        char currentByte = byteArray.at(i);
        for (int j=7; j>=0; j--) {
            char inp = currentByte >> j & 0x01;
            if (i == byteArray.length()-1 && j < 4) {
                inp = 0x00;
            }
            out = x[3];
            x[3] = x[2];
            x[2] = x[1];
            x[1] = out ^ x[0];
            x[0] = inp ^ out;
        }
    }

    char crc = ((x[3] << 3) | (x[2] << 2) | (x[1] << 1) | x[0]);
    if (crc != frame->crc) {
        return frame;
    }


    // sequence number
    frame->sequenceNumber = (((byteArray.at(currentIndex)) & 0xF0) >> 4) & 0x0F;


    // digital ports
    currentIndex -= 1;
    frame->digitalInputStates[0] = ((byteArray.at(currentIndex) & 0x80) >> 7) & 0x01;
    frame->digitalInputStates[1] = ((byteArray.at(currentIndex) & 0x40) >> 6) & 0x01;
    frame->digitalOutputStates[0] = ((byteArray.at(currentIndex) & 0x20) >> 5) & 0x01;
    frame->digitalOutputStates[1] = ((byteArray.at(currentIndex) & 0x10) >> 4) & 0x01;


    // analog data
    if (numChannels >= 1) {
        frame->analogData[0] = ((qint16)(((byteArray.at(currentIndex) & 0x0F) << 6) | ((byteArray.at(currentIndex-1) & 0xFC) >> 2))) & 0x03FF;
        currentIndex -= 1;
    }
    if (numChannels >= 2) {
        frame->analogData[1] = ((qint16)(((byteArray.at(currentIndex) & 0x03) << 8) | ((byteArray.at(currentIndex-1) & 0xFF)))) & 0x03FF;
        currentIndex -= 2;
    }
    if (numChannels >= 3) {
        frame->analogData[2] = ((qint16)(((byteArray.at(currentIndex) & 0xFF) << 2) | ((byteArray.at(currentIndex-1) & 0xC0) >> 6))) & 0x03FF;
        currentIndex -= 1;
    }
    if (numChannels >= 4) {
        frame->analogData[3] = ((qint16)(((byteArray.at(currentIndex) & 0x3F) << 4) | ((byteArray.at(currentIndex-1) & 0xF0) >> 4))) & 0x03FF;
        currentIndex -= 1;
    }
    if (numChannels >= 5) {
        frame->analogData[4] = ((qint16)(((byteArray.at(currentIndex) & 0x0F) << 2) | ((byteArray.at(currentIndex-1) & 0xC0) >> 6))) & 0x003F; // 6bit
        currentIndex -= 0;
    }
    if (numChannels >= 6) {
        frame->analogData[5] = ((qint16)(((byteArray.at(currentIndex) & 0x3F) << 2))) & 0x003F; // 6bit
        currentIndex -= 0;
    }


    // finalize
    frame->numChannels = numChannels;
    frame->isValid = true;


    return frame;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
QString ModalityBITalinoFrame::toString() {
    if (!isValid) return QString();

    QString string = "";

    string += QString::number((int)sequenceNumber) + " ";
    for (int i=0; i<numChannels; i++) {
        string += QString::number(analogData[i]);
        if (i < numChannels-1) {
            string += ",";
        }
    }

    return string;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ModalityBITalino::ModalityBITalino(QObject *parent) : Modality(parent) {
    type = "bitalino";


    // default configuration
    setSamplingRate(10);
    numAvailableAnalogChannels = 0;
    for (int i=0; i<6; i++) setAnalogChannelAvailability(i, false);


    // initialize bluetooth
    btDeviceDiscoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    btDeviceDiscoveryAgent->setLowEnergyDiscoveryTimeout(5000);

    QObject::connect(btDeviceDiscoveryAgent, SIGNAL(finished()), this, SLOT(slotBtDeviceDiscoveryFinished()));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool ModalityBITalino::initialize(QVariantMap configuration) {

    if (configuration.contains("sampling_rate")) {
        setSamplingRate(configuration.value("sampling_rate").toInt());
    }

    if (configuration.contains("analog_channels")) {
        QVariantList analogChannels = configuration.value("analog_channels").toList();
        for (int i=0; i<analogChannels.count(); i++) {
            setAnalogChannelAvailability(i, analogChannels.value(i).toBool());
        }
    }

    return Modality::initialize(configuration);
}
//---------------------------------------------------------------------------
void ModalityBITalino::reset() {
    disconnectFromDevice();

    Modality::reset();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void ModalityBITalino::slotBtDeviceDiscoveryFinished() {
    QList<QBluetoothDeviceInfo> devices = btDeviceDiscoveryAgent->discoveredDevices();
    foreach (QBluetoothDeviceInfo info, devices) {
        if (!info.name().toLower().startsWith("bitalino")) {
            continue;
        }
        // currently supporting only BLE devices
        if (!(info.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration)) {
            continue;
        }

        QVariantMap infoMap;
        infoMap.insert("name", info.name());
        infoMap.insert("address", info.address().toString());

        btDiscoveredDeviceList.append(infoMap);
        btDiscoveredDeviceInfoList.append(info);
    }

    emit deviceDiscovered(btDiscoveredDeviceList);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void ModalityBITalino::slotBtServiceCharacteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue) {
    //qDebug() << "changed" << characteristic.name() << newValue;
    if (characteristic.uuid() == QBluetoothUuid(MMR_MODALITY_BITALINO_FRAMES_UUID)) {
        ModalityBITalinoFrame *frame = ModalityBITalinoFrame::fromByteArray(newValue, numAvailableAnalogChannels);

        if (frame->isValid) {
            qint64 timestamp = getAcquisitionTimestamp();

            QByteArray byteArray;
            QDataStream outStream(&byteArray, QIODevice::WriteOnly);
            outStream.setVersion(QDataStream::Qt_5_9);

            outStream << frame->numChannels;

            for (int i=0; i<frame->numChannels; i++) {
                outStream << frame->analogData[i];
            }

            emit acquired(timestamp, byteArray);
        }

        frame->deleteLater();
    }
}
//---------------------------------------------------------------------------
void ModalityBITalino::slotBtServiceCharacteristicWritten(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue) {
    qDebug() << "written" << characteristic.name() << newValue;
}
//---------------------------------------------------------------------------
void ModalityBITalino::slotBtServiceStateChanged(QLowEnergyService::ServiceState newState) {
    if (newState == QLowEnergyService::ServiceDiscovered) {
        qDebug() << "detail discovery finished";
        QLowEnergyCharacteristic commandsCharacteristic = btExchangeService->characteristic(QBluetoothUuid(MMR_MODALITY_BITALINO_COMMANDS_UUID));
        if (!commandsCharacteristic.isValid()) {
            emit deviceConnectionFailed("characteristic");
            return;
        }

        QLowEnergyCharacteristic framesCharacteristic = btExchangeService->characteristic(QBluetoothUuid(MMR_MODALITY_BITALINO_FRAMES_UUID));
        if (!framesCharacteristic.isValid()) {
            emit deviceConnectionFailed("characteristic");
            return;
        }

        emit deviceConnected();
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool ModalityBITalino::startAcquisition() {
    if (!btLEController) return false;
    if (!btExchangeService) return false;

    QLowEnergyCharacteristic commandsCharacteristic = btExchangeService->characteristic(QBluetoothUuid(MMR_MODALITY_BITALINO_COMMANDS_UUID));
    QLowEnergyCharacteristic framesCharacteristic = btExchangeService->characteristic(QBluetoothUuid(MMR_MODALITY_BITALINO_FRAMES_UUID));

    // sampling rate
    QByteArray samplingRateData;
    char samplingRateByte = 0x03; // set sampling rate
    if (samplingRate < 10) {
        samplingRateByte = samplingRateByte | (0 << 6);
    }
    else if (samplingRate < 100) {
        samplingRateByte = samplingRateByte | (1 << 6);
    }
    else if (samplingRate < 1000) {
        samplingRateByte = samplingRateByte | (2 << 6);
    }
    else {
        samplingRateByte = samplingRateByte | (3 << 6);
    }
    samplingRateData.append(samplingRateByte);
    btExchangeService->writeCharacteristic(commandsCharacteristic, samplingRateData);

    // analog channels
    QByteArray channelData;
    char channelByte = 0x01; // live mode
    for (int i=0; i<6; i++) {
        if (analogChannelAvailabilities[i]) {
            channelByte = channelByte | (1 << (2 + i));
        }
    }
    channelData.append(channelByte);
    btExchangeService->writeCharacteristic(commandsCharacteristic, channelData);

    // enable notification
    QLowEnergyDescriptor configDescriptor = framesCharacteristic.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
    btExchangeService->writeDescriptor(configDescriptor, QByteArray::fromHex("0100"));

    return true;
}
//---------------------------------------------------------------------------
void ModalityBITalino::stopAcquisition() {
    if (!btLEController) return;
    if (!btExchangeService) return;

    QLowEnergyCharacteristic commandsCharacteristic = btExchangeService->characteristic(QBluetoothUuid(MMR_MODALITY_BITALINO_COMMANDS_UUID));
    QLowEnergyCharacteristic framesCharacteristic = btExchangeService->characteristic(QBluetoothUuid(MMR_MODALITY_BITALINO_FRAMES_UUID));

    // disable notification
    QLowEnergyDescriptor configDescriptor = framesCharacteristic.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
    btExchangeService->writeDescriptor(configDescriptor, QByteArray::fromHex("0000"));

    // change to idle mode
    QByteArray idleData;
    char idleByte = 0x00; // idle mode
    idleData.append(idleByte);
    btExchangeService->writeCharacteristic(commandsCharacteristic, idleData);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
QVariantList ModalityBITalino::parseData(QByteArray data) {
    QVariantList parsedDataList;

    QDataStream inStream(&data, QIODevice::ReadOnly);
    inStream.setVersion(QDataStream::Qt_5_9);

    int numChannels;
    inStream >> numChannels;
    parsedDataList.append(Modality::parsedDataItemWithValue("num_channels", numChannels));

    for (int i=0; i<numChannels; i++) {
        qint16 analogData;
        inStream >> analogData;
        parsedDataList.append(Modality::parsedDataItemWithValue(QString("c%1").arg(i), analogData));
    }

    return parsedDataList;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void ModalityBITalino::startDiscovery() {
    btDiscoveredDeviceList.clear();
    btDiscoveredDeviceInfoList.clear();

    btDeviceDiscoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}
//---------------------------------------------------------------------------
void ModalityBITalino::stopDiscovery() {
    btDeviceDiscoveryAgent->stop();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void ModalityBITalino::setSamplingRate(int rate) {
    samplingRate = rate;
}
//---------------------------------------------------------------------------
void ModalityBITalino::setAnalogChannelAvailability(int channel, bool availability) {
    if (channel < 0) return;
    if (channel >= 6) return;
    analogChannelAvailabilities[channel] = availability;

    numAvailableAnalogChannels = 0;
    for (int i=0; i<6; i++) {
        if (analogChannelAvailabilities[i]) {
            numAvailableAnalogChannels += 1;
        }
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void ModalityBITalino::connectToDevice(int index) {
    QBluetoothDeviceInfo info = btDiscoveredDeviceInfoList.at(index);

    disconnectFromDevice();

    btLEController = QLowEnergyController::createCentral(info);

    QObject::connect(btLEController, &QLowEnergyController::connected, this, [this]() {
        qDebug() << "controller connected";
        btLEController->discoverServices();
    });
    QObject::connect(btLEController, &QLowEnergyController::discoveryFinished, this, [this]() {
        qDebug() << "service discovery finished";

        // create service
        btExchangeService = btLEController->createServiceObject(QBluetoothUuid(MMR_MODALITY_BITALINO_EXCHANGE_UUID));
        if (!btExchangeService) {
            emit deviceConnectionFailed("create_service");
            return;
        }
        QObject::connect(btExchangeService, SIGNAL(characteristicChanged(QLowEnergyCharacteristic,QByteArray)), this, SLOT(slotBtServiceCharacteristicChanged(QLowEnergyCharacteristic,QByteArray)));
        QObject::connect(btExchangeService, SIGNAL(characteristicWritten(QLowEnergyCharacteristic,QByteArray)), this, SLOT(slotBtServiceCharacteristicWritten(QLowEnergyCharacteristic,QByteArray)));
        QObject::connect(btExchangeService, SIGNAL(stateChanged(QLowEnergyService::ServiceState)), this, SLOT(slotBtServiceStateChanged(QLowEnergyService::ServiceState)));

        // discover details
        btExchangeService->discoverDetails();
    });

    btLEController->connectToDevice();
}
//---------------------------------------------------------------------------
void ModalityBITalino::disconnectFromDevice() {
    if (btExchangeService) {
        btExchangeService->deleteLater();
        btExchangeService = 0;
    }

    if (btLEController) {
        btLEController->deleteLater();
        btLEController = 0;
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#endif // MMR_MODALITY_BITALINO
