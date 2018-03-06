import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

ModalityPage {
    id: container

    property ComboBox deviceComboBox
    property Button deviceDiscoverButton
    property Button deviceConnectButton

    property var modality

    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    function initialize() {
        modality = qMain.getClientModality(identifier);

        modality.deviceDiscovered.connect(slotDeviceDiscovered);
    }
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    function slotDeviceDiscovered(list) {
        console.log(list);
        deviceComboBox.devices = list;
        deviceDiscoverButton.enabled = true;
    }
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------

    content: ColumnLayout {
        Layout.fillWidth: true
        spacing: 8

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Text {
                text: "Device: "
            }
            ComboBox {
                id: deviceComboBox
                Layout.fillWidth: true
                model: []
                enabled: !container.isConnected

                property var devices: []

                Component.onCompleted: {
                    container.deviceComboBox = this;
                }

                onDevicesChanged: {
                    var modelTextList = [];
                    for (var i in devices) {
                        var device = devices[i];
                        modelTextList.push(device["name"] + " (" + device["address"] + ")");
                    }
                    model = modelTextList;
                }
            }
            Button {
                id: deviceDiscoverButton
                text: "Discover"

                Component.onCompleted: {
                    container.deviceDiscoverButton = this;
                }

                onClicked: {
                    deviceDiscoverButton.enabled = false;
                    deviceComboBox.devices = [];
                    container.modality.startDiscovery();
                }
            }
        }
        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Text {
                text: "Sampling rate: "
            }
            ComboBox {
                id: samplingRateComboBox
                Layout.fillWidth: true
                model: ["100", "10", "1"]
                enabled: !container.isConnected

                Component.onCompleted: {
                    //container.deviceComboBox = this;
                }
            }
        }
        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Text {
                text: "Channels: "
            }
            Flow {
                Layout.fillWidth: true
                spacing: 4

                Repeater {
                    id: analogChannelsRepeater
                    model: 6
                    CheckBox {
                        text: "A" + (index+1)
                    }
                }
            }
        }
        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Button {
                id: deviceConnectButton
                Layout.fillWidth: true
                text: "Connect"

                Component.onCompleted: {
                    container.deviceConnectButton = this;
                }

                onClicked: {
                    qMain.clientSetConfiguration(container.identifier, "sampling_rate", Number(samplingRateComboBox.currentText));

                    var analogChannels = [];
                    for (var i=0; i<6; i++) {
                        analogChannels.push(analogChannelsRepeater.itemAt(i).checked);
                    }
                    console.log(analogChannels);
                    qMain.clientSetConfiguration(container.identifier, "analog_channels", analogChannels);

                    container.modality.connectToDevice(deviceComboBox.currentIndex);
                }
            }
        }
    }
}
