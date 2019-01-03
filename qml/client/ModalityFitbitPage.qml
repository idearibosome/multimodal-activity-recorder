import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

ModalityPage {
    id: container

    property TextField serverPort
    property Button serverStartButton
    property Button serverStopButton

    property var modality

    property bool isServerRunning: false

    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    function initialize() {
        modality = qMain.getClientModality(identifier);
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
                text: "Port: "
            }
            TextField {
                id: serverPort
                Layout.fillWidth: true
                text: "27001"

                Component.onCompleted: {
                    container.serverPort = this;
                }
            }
        }
        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Button {
                id: serverStartButton
                Layout.fillWidth: true
                text: "Start"
                enabled: !isServerRunning

                Component.onCompleted: {
                    container.serverStartButton = this;
                }

                onClicked: {
                    container.modality.startServer(container.serverPort.text);
                    container.isServerRunning = true;
                }
            }
            Button {
                id: serverStopButton
                Layout.fillWidth: true
                text: "Stop"
                enabled: isServerRunning

                Component.onCompleted: {
                    container.serverStopButton = this;
                }

                onClicked: {
                    container.modality.stopServer();
                    container.isServerRunning = false;
                }
            }
        }
    }
}
