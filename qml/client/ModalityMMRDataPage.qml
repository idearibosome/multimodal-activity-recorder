import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3

ModalityPage {
    id: container

    onIdentifierChanged: {
    }

    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    function initialize() {
        var modality = qMain.getClientModality(identifier);
    }
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------

    content: ColumnLayout {
        Layout.fillWidth: true
        spacing: 8

        RowLayout {
            Layout.fillWidth: true
            spacing: 8
        }
    }
}
