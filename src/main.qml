import QtQuick 2.10
import QtQuick.Window 2.3
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.0
import codes.eeems.gameboy 1.0
import "."

ApplicationWindow {
    id: window
    objectName: "window"
    visible: stateController.state !== "loading"
    width: screenGeometry.width
    height: screenGeometry.height
    title: Qt.application.displayName
    Component.onCompleted: { controller.startup(); }
    header: Rectangle {
        color: "black"
        height: menu.height
        RowLayout {
            id: menu
            anchors.left: parent.left
            anchors.right: parent.right
            width: parent.width
            RowLayout {
                id: leftMenu
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft
                Clickable {
                    text: stateController.state === "picker" ? "⬅️" : "Exit"
                    color: "white"
                    topPadding: 5
                    bottomPadding: 5
                    leftPadding: 10
                    rightPadding: 10
                    onClicked: {
                        console.log("Back button pressed");
                        if(stateController.state === "picker"){
                            stateController.state = "loaded"
                            return;
                        }
                        if(stateController.state === "loaded"){
                            console.log("Quitting");
                            Qt.quit();
                            return;
                        }
                        console.log("Going back to main view");
                        stateController.state = "loaded";
                    }
                }
                Item { Layout.fillWidth: true; }
            }
            RowLayout {
                id: centerMenu
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignCenter
                Item { Layout.fillWidth: true; }
                Label {
                    id: title
                    color: "white"
                    text: stateController.state === "picker" ? "Select a ROM" : window.title
                }
                Item { Layout.fillWidth: true; }
            }
            RowLayout {
                id: rightMenu
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignRight
                Item { Layout.fillWidth: true; }
                Clickable {
                    text: "Open"
                    enabled: !gameboy.running
                    visible: stateController.state === "loaded"
                    color: enabled ? "white" : "black"
                    topPadding: 5
                    bottomPadding: 5
                    leftPadding: 10
                    rightPadding: 10
                    onClicked: stateController.state = "picker"
                }
                Clickable {
                    text: "Load"
                    enabled: picker.selectedIndex !== -1
                    visible: stateController.state === "picker"
                    color: enabled ? "white" : "black"
                    topPadding: 5
                    bottomPadding: 5
                    leftPadding: 10
                    rightPadding: 10
                    onClicked: {
                        gameboy.loadROM(picker.selected())
                        stateController.state = "loaded"
                    }
                }
            }
        }
    }
    contentData: [
        Rectangle {
            anchors.fill: parent
            color: "white"
        },
        Item {
            anchors.fill: parent
            enabled: gameboy.running && stateController.state === "loaded"

            ColumnLayout {
                anchors.bottom: gameboyContainer.bottom
                anchors.right: gameboyContainer.left
                anchors.rightMargin: 20
                width: 150
                Clickable {
                    text: "1x"
                    onClicked: gameboyContainer.scale = 1
                    border: 1
                    Layout.fillWidth: true
                }
                Clickable {
                    text: "2x"
                    onClicked: gameboyContainer.scale = 2
                    border: 1
                    Layout.fillWidth: true
                }
                Clickable {
                    text: "3x"
                    onClicked: gameboyContainer.scale = 3
                    border: 1
                    Layout.fillWidth: true
                }
                Clickable {
                    text: "4x"
                    onClicked: gameboyContainer.scale = 4
                    border: 1
                    Layout.fillWidth: true
                }
                Clickable {
                    text: "5x"
                    onClicked: gameboyContainer.scale = 5
                    border: 1
                    Layout.fillWidth: true
                }
            }

            Rectangle {
                id: gameboyContainer
                anchors.centerIn: parent
                color: "black"
                property int scale: 5
                width: 160 * 5
                height: 144 * 5
                Gameboy {
                    id: gameboy
                    anchors.centerIn: parent
                    width: 160 * parent.scale
                    height: 144 * parent.scale

                    Keys.onPressed: (event)=> {
                        gameboy.keyDown(event.key);
                    }
                    Keys.onReleased: (event)=> {
                        gameboy.keyUp(event.key);
                    }
                }
            }

            ColumnLayout {
                anchors.bottom: gameboyContainer.bottom
                anchors.left: gameboyContainer.right
                anchors.leftMargin: 20
                width: 150
                Clickable {
                    text: gameboy.paused ? "Resume" : "Pause"
                    onClicked: gameboy.toggle()
                    border: 1
                    Layout.fillWidth: true
                }
                Clickable {
                    text: "Stop"
                    onClicked: gameboy.stop()
                    border: 1
                    Layout.fillWidth: true
                }
                Clickable {
                    text: "⏩"
                    enabled: !gameboy.paused
                    onClicked: gameboy.toggleSpeed()
                    border: 1
                    Layout.fillWidth: true
                }
            }

            Clickable {
                id: buttonLeft
                text: "←"
                font.pointSize: 20
                width: height
                verticalAlignment: Qt.AlignTop
                border: 1
                radius: width / 2
                anchors.bottom: buttonDown.top
                anchors.right: buttonDown.left
                onPressed: gameboy.keyDown(Qt.Key_Left)
                onReleased: gameboy.keyUp(Qt.Key_Left)
            }
            Clickable {
                id: buttonUp
                text: "↑"
                font.pointSize: 20
                width: height
                border: 1
                radius: width / 2
                anchors.bottom: buttonLeft.top
                anchors.left: buttonDown.left
                onPressed: gameboy.keyDown(Qt.Key_Up)
                onReleased: gameboy.keyUp(Qt.Key_Up)
            }
            Clickable {
                id: buttonRight
                text: "→"
                font.pointSize: 20
                width: height
                verticalAlignment: Qt.AlignTop
                border: 1
                radius: width / 2
                anchors.bottom: buttonDown.top
                anchors.left: buttonDown.right
                onPressed: gameboy.keyDown(Qt.Key_Right)
                onReleased: gameboy.keyUp(Qt.Key_Right)
            }
            Clickable {
                id: buttonDown
                text: "↓"
                font.pointSize: 20
                width: height
                border: 1
                radius: width / 2
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 100
                anchors.left: parent.left
                anchors.leftMargin: 100 + buttonLeft.width
                onPressed: gameboy.keyDown(Qt.Key_Down)
                onReleased: gameboy.keyUp(Qt.Key_Down)
            }

            Clickable {
                id: buttonStart
                text: "start"
                font.pointSize: 15
                border: 1
                radius: 10
                anchors.bottom: buttonSelect.bottom
                anchors.left: buttonSelect.right
                anchors.leftMargin: 20
                onPressed: gameboy.keyDown(Qt.Key_Return)
                onReleased: gameboy.keyUp(Qt.Key_Return)
            }
            Clickable {
                id: buttonSelect
                text: "select"
                font.pointSize: 15
                border: 1
                radius: 10
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 50
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.horizontalCenterOffset: -(buttonSelect.width / 2) - (buttonStart.anchors.leftMargin / 2)
                onPressed: gameboy.keyDown(Qt.Key_Space)
                onReleased: gameboy.keyUp(Qt.Key_Space)
            }

            Clickable {
                id: buttonB
                text: "B"
                font.pointSize: 20
                border: 1
                radius: width / 2
                width: height
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 100
                anchors.right: buttonA.left
                anchors.rightMargin: 20
                onPressed: gameboy.keyDown(Qt.Key_X)
                onReleased: gameboy.keyUp(Qt.Key_X)
            }
            Clickable {
                id: buttonA
                text: "A"
                font.pointSize: 20
                border: 1
                radius: width / 2
                width: height
                anchors.right: parent.right
                anchors.rightMargin: 100
                anchors.bottom: buttonB.top
                onPressed: gameboy.keyDown(Qt.Key_Z)
                onReleased: gameboy.keyUp(Qt.Key_Z)
            }
        },
        FilePicker {
            id: picker
            visible: stateController.state === "picker"
            anchors.fill: parent
        }

    ]
    StateGroup {
        id: stateController
        objectName: "stateController"
        state: "loading"
        states: [
            State { name: "loaded" },
            State { name: "loading" },
            State { name: "picker" }
        ]
        transitions: [
            Transition {
                from: "*"; to: "loaded"
                SequentialAnimation {
                    ScriptAction { script: {
                        console.log("Display loaded");
                    } }
                }
            },
            Transition {
                from: "*"; to: "loading"
                SequentialAnimation {
                    ScriptAction { script: {
                        console.log("Loading display");
                        controller.startup();
                    } }
                }
            },
            Transition {
                from: "*"; to: "picker"
                SequentialAnimation {
                    ScriptAction { script: {
                        console.log("Showing file picker");
                        picker.selectedIndex = -1
                    } }
                }
            }
        ]
    }
}
