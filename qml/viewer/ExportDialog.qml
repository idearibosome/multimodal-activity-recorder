import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import QtQuick.Window 2.3

import IRQtModules 1.0 as IRQM

Window {
    id: container
    modality: Qt.ApplicationModal
    flags: Qt.Dialog | Qt.MSWindowsFixedSizeDialogHint | Qt.CustomizeWindowHint | Qt.WindowTitleHint | Qt.WindowCloseButtonHint | Qt.WindowSystemMenuHint
    title: "Export"
    width: 520
    height: 400
    minimumWidth: 520
    minimumHeight: 400
    visible: false

    property TextArea logTextArea
    property bool isFinished: false

    onClosing: {
        if (!isFinished) {
            close.accepted = false;
        }
    }

    Component.onCompleted: {
        IRQM.SignalHandler.bindSignal("main", "exportLog", this, "slotExportLog");
        IRQM.SignalHandler.bindSignal("main", "exportFinished", this, "slotExportFinished");
    }

    Component.onDestruction: {
        IRQM.SignalHandler.unbindAllSignalsForSlot(this);
    }

    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    function slotExportLog(text) {
        logTextArea.append(text);
    }
    //---------------------------------------------------------------------------
    function slotExportFinished() {
        isFinished = true;
    }
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        TextArea {
            id: logTextArea
            Layout.fillWidth: true
            Layout.fillHeight: true
            wrapMode: TextEdit.Wrap
            readOnly: true
            text: ""

            Component.onCompleted: {
                container.logTextArea = logTextArea;
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 6

            Item {
                Layout.fillWidth: true
            }

            Button {
                id: doneButton
                text: "Done"
                enabled: container.isFinished

                onClicked: {
                    close();
                }
            }
        }
    }


}
