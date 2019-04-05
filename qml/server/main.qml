import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
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

    property ListModel modalityListModel
    property ListModel recognizerListModel
    property var modalityList: ([])
    property var recognizerList: ([])

    property bool isConnectionOperating: false
    property bool isRunning: false

    Component.onCompleted: {
        IRQM.SignalHandler.bindSignal("main", "log", this, "log");

        IRQM.SignalHandler.bindSignal("mmrserver", "listening", this, "serverListening");
        IRQM.SignalHandler.bindSignal("mmrserver", "listeningFailed", this, "serverListeningFailed");
        IRQM.SignalHandler.bindSignal("mmrserver", "stopped", this, "serverStopped");

        IRQM.SignalHandler.bindSignal("mmrconnection", "modalityRegistered", this, "modalityConnectionRegistered");
        IRQM.SignalHandler.bindSignal("mmrconnection", "recognizerRegistered", this, "recognizerConnectionRegistered");
        IRQM.SignalHandler.bindSignal("mmrconnection", "modalityDisconnected", this, "modalityConnectionDisconnected");
        IRQM.SignalHandler.bindSignal("mmrconnection", "recognizerDisconnected", this, "recognizerConnectionDisconnected");
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
    function modalityConnectionRegistered(type, identifier) {
        modalityList.push({"type": type, "identifier": identifier});
        var text = type + " (" + identifier + ")";
        modalityListModel.append({"text": text});
    }
    //---------------------------------------------------------------------------
    function recognizerConnectionRegistered(name, identifier) {
        recognizerList.push({"name": name, "identifier": identifier});
        var text = name + " (" + identifier + ")";
        recognizerListModel.append({"text": text});
    }
    //---------------------------------------------------------------------------
    function modalityConnectionDisconnected(type, identifier) {
        var modalityIndex = -1;
        for (var i in modalityList) {
            if (modalityList[i]["identifier"] === identifier) {
                modalityIndex = i;
                break;
            }
        }

        if (modalityIndex >= 0) {
            modalityList.splice(modalityIndex, 1);
            modalityListModel.remove(modalityIndex);
        }
    }
    //---------------------------------------------------------------------------
    function recognizerConnectionDisconnected(name, identifier) {
        var recognizerIndex = -1;
        for (var i in recognizerList) {
            if (recognizerList[i]["identifier"] === identifier) {
                recognizerIndex = i;
                break;
            }
        }

        if (recognizerIndex >= 0) {
            recognizerList.splice(recognizerIndex, 1);
            recognizerListModel.remove(recognizerIndex);
        }
    }
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------

    FileDialog {
        id: saveDirDialog
        selectFolder: true
        onAccepted: {
            var path = fileUrl.toString();
            if (path.startsWith("file:///")) {
                path = path.replace(/^(file:\/{3})/,"");
            }
            else if (path.startsWith("file:")) {
                path = path.replace(/^(file:)/,"");
            }
            path = decodeURIComponent(path);

            storagePathTextField.text = path;
        }
    }

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
                            text: "Port (Modality):"
                        }
                        TextField {
                            id: serverModalityPortTextField
                            Layout.preferredWidth: 60
                            inputMethodHints: Qt.ImhDigitsOnly
                            text: "8880"
                        }
                        Text {
                            text: "Port (Recognizer):"
                        }
                        TextField {
                            id: serverRecognizerPortTextField
                            Layout.preferredWidth: 60
                            inputMethodHints: Qt.ImhDigitsOnly
                            text: "8881"
                        }
                        Button {
                            id: startServerButton
                            text: "Start Server"
                            enabled: !container.isConnectionOperating
                            visible: !container.isRunning

                            onClicked: {
                                container.isConnectionOperating = true;
                                mServer.startServer(Number(serverModalityPortTextField.text), Number(serverRecognizerPortTextField.text));
                            }
                        }
                        Button {
                            id: stopServerButton
                            text: "Stop Server"
                            enabled: !container.isConnectionOperating
                            visible: container.isRunning

                            onClicked: {
                                mServer.stopServer();
                            }
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
                                model: ListModel {
                                    id: modalityListModel

                                    Component.onCompleted: {
                                        container.modalityListModel = modalityListModel;
                                    }
                                }
                            }

                            Text {
                                Layout.preferredWidth: parent.width
                                horizontalAlignment: Text.AlignHCenter
                                text: "Recognizers"
                            }
                            TableView {
                                id: recognizerListView
                                Layout.preferredWidth: parent.width
                                Layout.fillHeight: true
                                Layout.minimumHeight: 100
                                headerVisible: false
                                alternatingRowColors: false
                                TableViewColumn {
                                    role: "text"
                                }
                                model: ListModel {
                                    id: recognizerListModel

                                    Component.onCompleted: {
                                        container.recognizerListModel = recognizerListModel;
                                    }
                                }
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
                                    text: "Browse"
                                    onClicked: {
                                        saveDirDialog.open();
                                    }
                                }

                                Button {
                                    text: "Set"
                                    onClicked: {
                                        mServer.setStorageBasePath(storagePathTextField.text);
                                    }
                                }
                            }

                            Button {
                                Layout.fillWidth: true
                                text: "Prepare Modalities"

                                onClicked: {
                                    mServer.prepareModalities();
                                }
                            }
                            Button {
                                Layout.fillWidth: true
                                text: "Start Acquisition"

                                onClicked: {
                                    mServer.startModalityAcquisition();
                                }
                            }
                            Button {
                                Layout.fillWidth: true
                                text: "Stop Acquisition"

                                onClicked: {
                                    mServer.stopModalityAcquisition();
                                }
                            }
                            Button {
                                Layout.fillWidth: true
                                text: "Finalize Modalities"

                                onClicked: {
                                    mServer.finalizeModalities();
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
