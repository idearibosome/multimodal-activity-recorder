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

    property bool isPlaying: false
    property alias loopPlay: repeatCheckBox.checked
    property var playbackLastFetchedTimestamp: -1

    Component.onCompleted: {
        IRQM.SignalHandler.bindSignal("main", "dataLoaded", this, "slotDataLoaded");
        IRQM.SignalHandler.bindSignal("main", "dataUnloaded", this, "slotDataUnloaded");
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
    function slotDataUnloaded() {
        container.numObjects = 0;

        while (modalityViewList.length > 0) {
            var modalityView = modalityViewList.pop();
            modalityView.parent = null;
            modalityView.destroy();
            modalityView = null;
        }

        timestampSlider.maximumValue = 0.0;
        timestampSlider.value = 0.0;
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

        Modality {}
    }

    Timer {
        id: playbackTimer
        interval: 100
        repeat: true
        running: container.isPlaying

        onTriggered: {
            var timestamp = qMain.getCurrentTimestamp();
            var elapsed = (container.playbackLastFetchedTimestamp >= 0 ? timestamp - container.playbackLastFetchedTimestamp : 0);
            var newPos = Math.min(timestampSlider.maximumValue, timestampSlider.value + elapsed);

            if (newPos >= timestampSlider.maximumValue) {
                if (container.loopPlay) {
                    newPos = 0;
                }
                else {
                    container.isPlaying = false;
                }
            }

            container.playbackLastFetchedTimestamp = timestamp;
            timestampSlider.value = newPos;
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
                id: playButton
                Layout.preferredWidth: 60
                text: "Play"
                visible: !(container.isPlaying)
                onClicked: {
                    container.playbackLastFetchedTimestamp = -1;
                    container.isPlaying = true;
                }
            }
            Button {
                id: pauseButton
                Layout.preferredWidth: 60
                text: "Pause"
                visible: container.isPlaying
                onClicked: {
                    container.isPlaying = false;
                }
            }
            CheckBox {
                id: repeatCheckBox
                text: "Repeat"
            }
            Button {
                Layout.preferredWidth: 40
                text: "<"
                onClicked: {
                    timestampSlider.value = Math.max(timestampSlider.value-100, 0);
                }
            }
            Button {
                Layout.preferredWidth: 40
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
                    if (playbackTimer.running) {
                        valueChangeEvent();
                    }
                    else {
                        timestampSliderTimer.start();
                    }
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
