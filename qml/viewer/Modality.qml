import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

import IRQtModules 1.0 as IRQM

import "../components" as Comp

ColumnLayout {
    id: container
    Layout.fillWidth: true
    spacing: 0

    property int objectIndex: 0
    property string objectName: ""
    property var modalityObject
    property var dataViewList: ([])

    Component.onCompleted: {
    }

    Component.onDestruction: {
        IRQM.SignalHandler.unbindAllSignalsForSlot(this);
    }

    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    function initialize() {
        modalityObject = qMain.getObjectAt(objectIndex);

        container.objectName = modalityObject.getObjectName();
    }
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    function slotDataLoaded() {
        var length = qMain.getFileMetadata().getLength();
        timestampSlider.maximumValue = length;

        container.numObjects = qMain.getNumObjects();
    }
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    function showModalityData() {
        while (dataViewList.length > 0) {
            var dataView = dataViewList.pop();
            dataView.parent = null;
            dataView.destroy();
            dataView = null;
        }

        var modalityDataList = modalityObject.getModalityDataList();
        for (var i=0; i<modalityDataList.length; i++) {
            var dataName = modalityDataList[i]["name"];
            var dataType = modalityDataList[i]["type"];

            if (dataType === "value") {
                var valueView = valueViewComponent.createObject(gridView);
                valueView.dataName = dataName;
                valueView.dataValue = modalityDataList[i]["value"];

                dataViewList.push(valueView);
            }
            else if (dataType === "image") {
                var imageView = imageViewComponent.createObject(gridView);
                imageView.dataName = dataName;
                imageView.imageSource = "image://object/" + objectIndex + "/" + dataName + "/" + Math.random();

                dataViewList.push(imageView);
            }
        }
    }
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    function showImageViewer(dataName) {
        var imageViewer = Qt.createComponent("ImageViewer.qml").createObject(container);
        imageViewer.setModalityImage(container.objectName, container.objectIndex, dataName);
        imageViewer.show();
    }
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------

    Component {
        id: valueViewComponent

        ColumnLayout {
            spacing: 0

            property string dataName
            property var dataValue

            Rectangle {
                Layout.fillWidth: true
                Layout.minimumWidth: 60
                Layout.preferredHeight: 20
                color: "#f8f8f8"

                Text {
                    anchors.fill: parent
                    anchors.margins: 2
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                    text: dataName
                }
            }
            Text {
                id: dataText
                text: dataValue !== undefined ? dataValue : ""
            }
        }
    }
    Component {
        id: imageViewComponent

        ColumnLayout {
            spacing: 0

            property string dataName
            property alias imageSource: dataImage.source

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 20
                color: "#f8f8f8"

                Text {
                    anchors.fill: parent
                    anchors.margins: 2
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                    text: dataName
                }
            }
            Image {
                id: dataImage
                Layout.preferredWidth: 400
                Layout.preferredHeight: 300
                cache: false
                fillMode: Image.PreserveAspectFit

                MouseArea {
                    anchors.fill: parent
                    onDoubleClicked: {
                        container.showImageViewer(dataName);
                    }
                }
            }
        }
    }

    Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: 20
        color: "#e8e8e8"

        Text {
            id: modalityNameText
            anchors.fill: parent
            anchors.margins: 2
            font.bold: true
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            text: container.objectName
        }
    }

    Grid {
        id: gridView
        Layout.fillWidth: true
        columns: 4
        columnSpacing: 4
        rowSpacing: 4
    }
}
