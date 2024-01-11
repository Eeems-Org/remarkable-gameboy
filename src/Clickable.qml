import QtQuick 2.10
import QtQuick.Controls 2.4

Label {
    id: root
    padding: 5
    verticalAlignment: Qt.AlignVCenter
    horizontalAlignment: Qt.AlignHCenter
    property int border: 0
    property int radius: 0
    property string borderColor: "black"
    property string backgroundColor: "transparent"
    signal pressed()
    signal released()
    signal clicked()
    MouseArea {
        anchors.fill: parent
        onPressed: root.pressed()
        onReleased: root.released()
        onClicked: root.clicked()
    }
    background: Rectangle{
        border.width: root.border
        border.color: root.borderColor
        radius: root.radius
        color: root.backgroundColor
    }
}
