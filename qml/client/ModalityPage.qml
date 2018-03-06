import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3

import IRQtModules 1.0 as IRQM

import "../components" as Comp

Item {
    id: container
    anchors.fill: parent

    property string title
    property string identifier

    property bool isConnectionOperating: false
    property bool isConnected: false

    property var content

    Component.onCompleted: {
        IRQM.SignalHandler.bindSignal("mmrclient", "serverConnected", this, "serverConnected");
        IRQM.SignalHandler.bindSignal("mmrclient", "serverDisconnected", this, "serverDisconnected");
        IRQM.SignalHandler.bindSignal("mmrclient", "serverError", this, "serverError");
    }

    Component.onDestruction: {
        IRQM.SignalHandler.unbindAllSignalsForSlot(this);
    }

    onContentChanged: {
        if (content) {
            content.parent = mainLayout;
        }
    }

    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    function initialize() {} // virtual
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    function serverConnected(identifier) {
        if (identifier !== container.identifier) return;
        isConnectionOperating = false;
        isConnected = true;
    }
    //---------------------------------------------------------------------------
    function serverDisconnected(identifier) {
        if (identifier !== container.identifier) return;
        isConnectionOperating = false;
        isConnected = false;
    }
    //---------------------------------------------------------------------------
    function serverError(identifier, errorCode, errorString) {
        isConnectionOperating = false;
    }
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------

    MessageDialog {
        id: deleteConfirmDialog
        icon: StandardIcon.Question
        standardButtons: StandardButton.Yes | StandardButton.No
        text: "Are you sure you want to delete this modality?"
        onYes: {
            IRQM.SignalHandler.sendSignal("main", "destroyClient", container.identifier);
        }
    }
    MessageDialog {
        id: disconnectConfirmDialog
        icon: StandardIcon.Question
        standardButtons: StandardButton.Yes | StandardButton.No
        text: "Are you sure you want to disconnect from the server?"
        onYes: {
            qMain.clientDisconnectServer(container.identifier);
        }
    }

    ScrollView {
        id: scrollView
        anchors.fill: parent
        horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
        verticalScrollBarPolicy: Qt.ScrollBarAlwaysOn

        Comp.Flickable {
            id: flickable
            anchors.fill: parent
            anchors.margins: 8
            contentWidth: width
            contentHeight: mainLayout.implicitHeight + (anchors.margins * 2)

            ColumnLayout {
                id: mainLayout
                width: flickable.contentWidth
                spacing: 8

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    Text {
                        Layout.fillWidth: true
                        font.bold: true
                        text: container.title + " (" + container.identifier + ")"
                        wrapMode: Text.Wrap
                    }
                    Button {
                        text: "Delete"
                        onClicked: {
                            deleteConfirmDialog.open();
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    Text {
                        text: "Server: "
                    }
                    TextField {
                        id: serverAddressTextField
                        Layout.fillWidth: true
                        text: "ws://"
                    }
                    Button {
                        id: connectButton
                        text: "Connect"
                        enabled: !container.isConnectionOperating
                        visible: !container.isConnected

                        onClicked: {
                            container.isConnectionOperating = true;
                            qMain.clientConnectServer(container.identifier, serverAddressTextField.text);
                        }
                    }
                    Button {
                        id: disconnectButton
                        text: "Disconnect"
                        enabled: !container.isConnectionOperating
                        visible: container.isConnected

                        onClicked: {
                            disconnectConfirmDialog.open();
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 1
                    color: "#cccccc"
                }
            }
        }
    }
}
