import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

ModalityPage {
    id: container

    property ComboBox targetSensorComboBox

    onIdentifierChanged: {
        if (targetSensorComboBox) {
            targetSensorComboBox.updateConfiguration();
        }
    }

    content: ColumnLayout {
        Layout.fillWidth: true
        spacing: 8

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Text {
                text: "Target: "
            }
            ComboBox {
                id: targetSensorComboBox
                Layout.fillWidth: true
                model: []
                enabled: !container.isConnected

                property var sensors: []

                Component.onCompleted: {
                    container.targetSensorComboBox = this;

                    sensors = qMain.modalityQtSensorGetAvailableSensors();

                    var modelTextList = [];
                    for (var i in sensors) {
                        var sensor = sensors[i];
                        modelTextList.push(sensor["text"] + " (" + sensor["identifier"] + ")");
                    }

                    model = modelTextList;

                    updateConfiguration();
                }

                function updateConfiguration() {
                    if (container.identifier.length <= 0) return;
                    if (currentIndex < 0) return;
                    if (currentIndex >= sensors.length) return;

                    qMain.clientSetConfiguration(container.identifier, "sensor", sensors[currentIndex]);
                }

                onSensorsChanged: {
                    updateConfiguration();
                }
                onCurrentIndexChanged: {
                    updateConfiguration();
                }
            }
        }
    }
}
