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
    title: "MMRClient"

    property var clientIdentifiers: []

    property var componentsOfModalityTypes: ({
        "kinect": componentModalityKinect,
        "qtsensor": componentModalityQtSensor,
        "bitalino": componentModalityBITalino
    })

    property TextArea logTextArea

    Component.onCompleted: {
        IRQM.SignalHandler.bindSignal("main", "log", this, "log");

        IRQM.SignalHandler.bindSignal("main", "destroyClient", this, "destroyClient");
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
    function createClient(modality) {
        var identifier = qMain.createClient(modality);
        if (!identifier) return;

        var text = modality["text"];
        var type = modality["type"];

        var tabObject = tabView.addTab(text);
        tabObject.anchors.fill = Qt.binding(function() { return tabObject.parent; });

        clientIdentifiers.push(identifier);

        var modalityObject = componentsOfModalityTypes[type].createObject(tabObject);
        if (!modalityObject) return;

        modalityObject.title = text;
        modalityObject.identifier = identifier;

        modalityObject.initialize();

        tabView.currentIndex = tabView.count - 1;
    }
    //---------------------------------------------------------------------------
    function destroyClient(identifier) {
        var tabIndex = clientIdentifiers.indexOf(identifier) + 1;
        if (tabIndex <= 0) return;

        console.log("destroyClient");
        console.log(identifier);

        qMain.destroyClient(identifier);
        tabView.removeTab(tabIndex);

        clientIdentifiers.splice(tabIndex, 1);
    }
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------

    Component {
        id: componentModalityKinect
        ModalityKinectPage {}
    }
    Component {
        id: componentModalityQtSensor
        ModalityQtSensorPage {}
    }
    Component {
        id: componentModalityBITalino
        ModalityBITalinoPage {}
    }

    SplitView {
        anchors.fill: parent
        orientation: Qt.Vertical

        TabView {
            id: tabView
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 8

            Tab {
                id: mainTab
                anchors.fill: parent
                title: "Main"

                ScrollView {
                    id: mainScrollView
                    anchors.fill: parent
                    horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
                    verticalScrollBarPolicy: Qt.ScrollBarAlwaysOn

                    Comp.Flickable {
                        id: mainFlickable
                        anchors.fill: parent
                        anchors.margins: 8

                        ColumnLayout {
                            width: mainFlickable.width
                            spacing: 8

                            RowLayout {
                                Layout.fillWidth: true
                                spacing: 8

                                Text {
                                    text: "New modality: "
                                }
                                ComboBox {
                                    id: newModalityComboBox
                                    Layout.fillWidth: true
                                    model: []

                                    property var modalities: []

                                    Component.onCompleted: {
                                        modalities = qMain.getAvailableModalities();

                                        var modelTextList = [];
                                        for (var i in modalities) {
                                            var modality = modalities[i];
                                            modelTextList.push(modality["text"]);
                                        }

                                        model = modelTextList;
                                    }
                                }
                                Button {
                                    text: "Add"

                                    onClicked: {
                                        if (newModalityComboBox.currentIndex < 0) return;
                                        var modality = newModalityComboBox.modalities[newModalityComboBox.currentIndex];
                                        container.createClient(modality);
                                    }
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
            text: "MultiModalRecorderClient"

            Component.onCompleted: {
                container.logTextArea = logTextArea
            }
        }
    }
}
