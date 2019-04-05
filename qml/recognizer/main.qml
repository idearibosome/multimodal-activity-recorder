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
    title: "MMRRecognizer"

    property TextArea logTextArea

    Component.onCompleted: {
        IRQM.SignalHandler.bindSignal("main", "log", this, "log");
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

    FileDialog {
        id: configDialog
        onAccepted: {
            var path = fileUrl.toString();
            if (path.startsWith("file:///")) {
                path = path.replace(/^(file:\/{3})/,"");
            }
            else if (path.startsWith("file:")) {
                path = path.replace(/^(file:)/,"");
            }
            path = decodeURIComponent(path);

            configPathTextField.text = path;
        }
    }
    ColumnLayout {
        anchors.fill: parent
        spacing: 8

        RowLayout {
            Layout.fillWidth: true
            Layout.margins: 8
            spacing: 8

            Text {
                text: "Config Path: "
            }
            TextField {
                id: configPathTextField
                Layout.fillWidth: true
                text: ""
            }
            Button {
                text: "Browse"
                onClicked: {
                    configDialog.open();
                }
            }

            Button {
                text: "Start"
                onClicked: {
                }
            }
        }
        TextArea {
            id: logTextArea
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 8
            wrapMode: TextEdit.Wrap
            readOnly: true
            text: "MultiModalRecorderRecognizer"

            Component.onCompleted: {
                container.logTextArea = logTextArea
            }
        }
    }
}
