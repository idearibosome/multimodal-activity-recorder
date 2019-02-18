import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import QtQuick.Window 2.3

Window {
    id: container
    width: 640
    height: 480
    flags: Qt.Window
    title: "Image Viewer"
    visible: false

    property string objectName: ""
    property int objectIndex: 0
    property string dataName: ""
    property string imageSource

    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    function setModalityImage(objectName, objectIndex, dataName) {
        container.title = objectName + " - " + dataName;
        container.objectName = objectName;
        container.objectIndex = objectIndex;
        container.dataName = dataName;

        refreshImage();
    }
    //---------------------------------------------------------------------------
    function refreshImage() {
        container.imageSource = "image://object/" + container.objectIndex + "/" + container.dataName + "/" + Math.random();
    }
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------


    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Button {
                id: refreshButton
                text: "Refresh"
                onClicked: {
                    container.refreshImage();
                }
            }
            Button {
                id: saveButton
                text: "Save"
                onClicked: {
                    container.refreshImage();
                    saveDialog.open();
                }

                FileDialog {
                    id: saveDialog
                    nameFilters: ["*.png", "*.jpg"]
                    selectExisting: false

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

                        qMain.getObjectAt(objectIndex).saveModalityImageData(container.dataName, path);
                    }
                }
            }

            Item {
                Layout.fillWidth: true
            }

        }
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 1
            color: "#888888"
        }
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            Image {
                source: container.imageSource
            }
        }
    }
}
