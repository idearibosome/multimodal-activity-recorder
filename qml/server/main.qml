import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import QtQuick.Window 2.3

import IRQtModules 1.0 as IRQM

import "../components" as Comp

Window {
    id: container
    visible: true
    width: 640
    height: 480
    title: "MMRServer"

    property TextArea logTextArea

    property bool isConnectionOperating: false
    property bool isRunning: false

    Component.onCompleted: {
        IRQM.SignalHandler.bindSignal("main", "log", this, "log");

        IRQM.SignalHandler.bindSignal("mmrserver", "listening", this, "serverListening");
        IRQM.SignalHandler.bindSignal("mmrserver", "listeningFailed", this, "serverListeningFailed");
        IRQM.SignalHandler.bindSignal("mmrserver", "stopped", this, "serverStopped");

        IRQM.SignalHandler.bindSignal("mmrconnection", "receivedData", this, "connectionReceivedData");
    }

    Component.onDestruction: {
        IRQM.SignalHandler.unbindAllSignalsForSlot(this);
    }

    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    function log(text) {
        logTextArea.append(text);
    }
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    function serverListening() {
        isConnectionOperating = false;
        isRunning = true;
    }
    //---------------------------------------------------------------------------
    function serverListeningFailed() {
        isConnectionOperating = false;
    }
    //---------------------------------------------------------------------------
    function serverStopped() {
        isConnectionOperating = false;
        isRunning = false;
    }
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    function connectionReceivedData(identifier, size) {
    }
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------

    SplitView {
        anchors.fill: parent
        orientation: Qt.Vertical

        ScrollView {
            id: mainScrollView
            Layout.fillWidth: true
            Layout.fillHeight: true
            horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
            verticalScrollBarPolicy: Qt.ScrollBarAlwaysOn

            Comp.Flickable {
                id: mainFlickable
                anchors.fill: parent
                anchors.margins: 8
                contentWidth: width
                contentHeight: mainLayout.implicitHeight + (anchors.margins * 2)

                ColumnLayout {
                    id: mainLayout
                    width: mainFlickable.width
                    spacing: 8

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        Text {
                            font.bold: true
                            text: "MMRServer"
                        }
                        Item {
                            Layout.fillWidth: true
                        }

                        Text {
                            text: "Port: "
                        }
                        TextField {
                            id: serverPortTextField
                            Layout.preferredWidth: 100
                            inputMethodHints: Qt.ImhDigitsOnly
                            text: "8880"
                        }
                        Button {
                            id: startServerButton
                            text: "Start Server"
                            enabled: !container.isConnectionOperating
                            visible: !container.isRunning

                            onClicked: {
                                container.isConnectionOperating = true;
                                mServer.startServer(Number(serverPortTextField.text));
                            }
                        }
                        Button {
                            id: stopServerButton
                            text: "Stop Server"
                            enabled: !container.isConnectionOperating
                            visible: container.isRunning
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignTop
                        spacing: 8

                        ColumnLayout {
                            Layout.preferredWidth: width
                            Layout.maximumWidth: width
                            width: 200
                            spacing: 8

                            Text {
                                Layout.preferredWidth: parent.width
                                horizontalAlignment: Text.AlignHCenter
                                text: "Modalities"
                            }
                            TableView {
                                id: modalityListView
                                Layout.preferredWidth: parent.width
                                Layout.fillHeight: true
                                Layout.minimumHeight: 200
                                headerVisible: false
                                alternatingRowColors: false
                                TableViewColumn {
                                    role: "text"
                                }
                                model: ListModel {}
                            }
                        }
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 8

                            RowLayout {
                                Layout.fillWidth: true
                                spacing: 8

                                Text {
                                    text: "Storage Path: "
                                }
                                TextField {
                                    id: storagePathTextField
                                    Layout.fillWidth: true
                                    text: ""
                                }
                                Button {
                                    text: "Set"
                                    onClicked: {
                                        mServer.setStorageBasePath(storagePathTextField.text);
                                    }
                                }
                            }

                            RowLayout {
                                Layout.fillWidth: true
                                spacing: 8

                                Text {
                                    text: "MariaDB Host: "
                                }
                                TextField {
                                    Layout.fillWidth: true
                                    text: ""
                                }
                                Button {
                                    text: "Connect"
                                }
                            }

                            Button {
                                Layout.fillWidth: true
                                text: "Prepare Modalities"

                                onClicked: {
                                    mServer.requestPrepareModalities();
                                }
                            }
                            Button {
                                Layout.fillWidth: true
                                text: "Start Acquisition"

                                onClicked: {
                                    mServer.requestStartModalities();
                                }
                            }
                            Button {
                                Layout.fillWidth: true
                                text: "Stop Acquisition"

                                onClicked: {
                                    mServer.requestStopModalities();
                                }
                            }
                            Button {
                                Layout.fillWidth: true
                                text: "Finalize Modalities"

                                onClicked: {
                                    mServer.requestFinalizeModalities();
                                }
                            }
                        }
                    }
                }
            }
        }

        TextArea {
            id: logTextArea
            Layout.fillWidth: true
            Layout.preferredHeight: 120
            Layout.margins: 8
            wrapMode: TextEdit.Wrap
            readOnly: true
            text: "MultiModalRecorderServer"

            Component.onCompleted: {
                container.logTextArea = logTextArea
            }
        }
    }
}
