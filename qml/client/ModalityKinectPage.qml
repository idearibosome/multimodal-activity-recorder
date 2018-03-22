import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

ModalityPage {
    id: container

    property ComboBox colorResolutionComboBox

    onIdentifierChanged: {
        if (colorResolutionComboBox) {
            colorResolutionComboBox.updateConfiguration();
        }
    }

    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    function initialize() {
    }
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------

    content: ColumnLayout {
        Layout.fillWidth: true
        spacing: 8

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Text {
                text: "Color Res.: "
            }
            ComboBox {
                id: colorResolutionComboBox
                Layout.fillWidth: true
                model: ["320x180", "480x270", "640x360", "720x480", "1280x720", "1920x1080"]
                currentIndex: 5
                enabled: !container.isConnected

                property var heights: [180, 270, 360, 480, 720, 1080]

                Component.onCompleted: {
                    container.colorResolutionComboBox = this;
                    updateConfiguration();
                }

                function updateConfiguration() {
                    if (container.identifier.length <= 0) return;
                    if (currentIndex < 0) return;

                    qMain.clientSetConfiguration(container.identifier, "color_height", heights[currentIndex]);
                }

                onCurrentIndexChanged: {
                    updateConfiguration();
                }
            }
        }
    }
}
