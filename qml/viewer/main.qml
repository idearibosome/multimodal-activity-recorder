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
    title: "MMRViewer"

    property int numObjects: 0
    property var modalityViewList: ([])
    property string filePath

    Component.onCompleted: {
        IRQM.SignalHandler.bindSignal("main", "dataLoaded", this, "slotDataLoaded");
    }

    Component.onDestruction: {
        IRQM.SignalHandler.unbindAllSignalsForSlot(this);
    }

    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    function slotDataLoaded() {
        var length = qMain.getFileMetadata().getLength();
        timestampSlider.maximumValue = length;

        container.numObjects = qMain.getNumObjects();
        for (var i=0; i<numObjects; i++) {
            var modalityView = modalityComponent.createObject(modalityLayout);
            modalityView.objectIndex = i;
            modalityView.initialize();

            modalityViewList.push(modalityView);
        }
    }
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    function loadObjectData(timestamp) {
        qMain.loadObjectData(timestamp);

        for (var i=0; i<modalityViewList.length; i++) {
            modalityViewList[i].showModalityData();
        }
    }
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------

    Component {
        id: modalityComponent

        Modality {

        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Text {
                id: filePathText
                Layout.fillWidth: true
                elide: Text.ElideRight
                text: container.filePath
            }
            Button {
                id: openButton
                text: "Open"
                onClicked: {
                    openDialog.open();
                }

                FileDialog {
                    id: openDialog
                    nameFilters: ["mmr.sqlite"]

                    onAccepted: {
                        var path = fileUrl.toString();
                        if (path.startsWith("file:///")) {
                            path = path.replace(/^(file:\/{3})/, "");
                        }
                        else if (path.startsWith("file:")) {
                            path = path.replace(/^(file:)/, "");
                        }
                        path = path.replace(/mmr\.sqlite$/, "");
                        path = decodeURIComponent(path);

                        container.filePath = path;
                        qMain.loadMMRData(path);
                    }
                }
            }

        }
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 1
            color: "#888888"
        }
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ScrollView {
                id: modalityScrollView
                anchors.fill: parent
                horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
                verticalScrollBarPolicy: Qt.ScrollBarAlwaysOn

                Comp.Flickable {
                    id: modalityFlickable
                    anchors.fill: parent
                    contentWidth: width
                    contentHeight: modalityLayout.implicitHeight + (anchors.margins * 2)

                    ColumnLayout {
                        id: modalityLayout
                        width: modalityFlickable.width
                        spacing: 8
                    }
                }
            }

        }
        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            TextField {
                id: timestampTextField
                Layout.preferredWidth: 80
                text: "0"
                validator: IntValidator {
                    id: timestampTextFieldValidator
                    bottom: timestampSlider.minimumValue
                    top: timestampSlider.maximumValue
                }
                onAccepted: {
                    timestampSlider.value = Number(text);
                }
            }
            Button {
                Layout.preferredWidth: 48
                text: "<"
                onClicked: {
                    timestampSlider.value = Math.max(timestampSlider.value-100, 0);
                }
            }
            Button {
                Layout.preferredWidth: 48
                text: ">"
                onClicked: {
                    timestampSlider.value = Math.min(timestampSlider.value+100, timestampSlider.maximumValue);
                }
            }
            Slider {
                id: timestampSlider
                Layout.fillWidth: true
                minimumValue: 0
                maximumValue: 0
                stepSize: 1

                function valueChangeEvent() {
                    timestampTextField.text = "" + Math.floor(value);
                    loadObjectData(value);
                }

                onValueChanged: {
                    timestampSliderTimer.start();
                }

                Timer {
                    id: timestampSliderTimer
                    interval: 100; running: false; repeat: false;
                    onTriggered: {
                        timestampSlider.valueChangeEvent();
                    }
                }
            }
            Text {
                id: maxTimestampText
                text: timestampSlider.maximumValue
            }
        }
    }

}
