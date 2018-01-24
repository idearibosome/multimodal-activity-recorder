import QtQuick 2.9
import QtQuick.Controls 1.4

Flickable {
    id: container

    boundsBehavior: (Qt.platform.os === "ios" || Qt.platform.os === "osx") ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds
}
