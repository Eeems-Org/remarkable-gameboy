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
    width: Screen.width
    height: Screen.height
    minimumWidth: gameboyContainer.width + speedButtons.width + stateButtons.width + 50
    minimumHeight: gameboyContainer.height + buttonSelect.height + 50
    title: gameboy.running ? gameboy.romName : Qt.application.displayName
    Component.onCompleted: stateController.state = "loaded"
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
                    id: backButton
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
                id: rightMenu
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignRight
                Item { Layout.fillWidth: true; }
                Clickable {
                    id: openButton
                    text: "Open"
                    visible: stateController.state === "loaded"
                    color: "white"
                    topPadding: 5
                    bottomPadding: 5
                    leftPadding: 10
                    rightPadding: 10
                    onClicked: stateController.state = "picker"
                }
                Clickable {
                    id: loadButton
                    text: picker.currentIndex > 0 && picker.get(picker.currentIndex, "fileIsDir") ? "Open" : "Load"
                    enabled: picker.currentIndex > 0
                    visible: stateController.state === "picker"
                    color: enabled ? "white" : "black"
                    topPadding: 5
                    bottomPadding: 5
                    leftPadding: 10
                    rightPadding: 10
                    onClicked: {
                        if(picker.currentIndex == -1){
                            return;
                        }
                        if(picker.get(picker.currentIndex, "fileIsDir")){
                            picker.folder = picker.get(picker.currentIndex, "fileUrl");
                            console.log(picker.folder);
                            return;
                        }
                        gameboy.loadROM(picker.get(picker.currentIndex, "filePath"));
                        stateController.state = "loaded";
                    }
                }
            }
        }
        RowLayout {
            id: centerMenu
            anchors.centerIn: parent
            Layout.alignment: Qt.AlignCenter
            Label {
                id: title
                color: "white"
                text: stateController.state === "picker" ? "Select a ROM" : window.title
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
            enabled: stateController.state === "loaded"

            ColumnLayout {
                id: speedButtons
                anchors.bottom: {
                    if(gameboyContainer.bottom < buttonUp.top){
                        return gameboyContainer.bottom
                    }
                    return buttonUp.top
                }
                anchors.bottomMargin: {
                    if(gameboyContainer.bottom < buttonUp.top){
                        return 0
                    }
                    return 20
                }
                anchors.right: gameboyContainer.left
                anchors.rightMargin: 20
                width: 180
                Clickable {
                    text: "1x"
                    onClicked: gameboyContainer.scale = 1
                    border: 1
                    Layout.fillWidth: true
                    backgroundColor: "white"
                }
                Clickable {
                    text: "2x"
                    onClicked: gameboyContainer.scale = 2
                    border: 1
                    Layout.fillWidth: true
                    backgroundColor: "white"
                }
                Clickable {
                    text: "3x"
                    onClicked: gameboyContainer.scale = 3
                    border: 1
                    Layout.fillWidth: true
                    backgroundColor: "white"
                }
                Clickable {
                    text: "4x"
                    onClicked: gameboyContainer.scale = 4
                    border: 1
                    Layout.fillWidth: true
                    backgroundColor: "white"
                }
                Clickable {
                    text: "5x"
                    onClicked: gameboyContainer.scale = 5
                    border: 1
                    Layout.fillWidth: true
                    backgroundColor: "white"
                }
                Clickable {
                    text: gameboy.greyscale ? "Greyscale" : "B&W"
                    color: "black"
                    backgroundColor: gameboy.greyscale ? "grey" : "white"
                    onClicked: gameboy.greyscale = !gameboy.greyscale
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
                activeFocusOnTab: true
                Gameboy {
                    id: gameboy
                    anchors.centerIn: parent
                    width: 160 * parent.scale
                    height: 144 * parent.scale
                    focus: true
                    Keys.onPressed: (event)=> gameboy.keyDown(event.key)
                    Keys.onReleased: (event)=> gameboy.keyUp(event.key)
                }
            }

            ColumnLayout {
                id: stateButtons
                anchors.bottom: {
                    if(gameboyContainer.bottom < buttonA.top){
                        return gameboyContainer.bottom
                    }
                    return buttonA.top
                }
                anchors.bottomMargin: {
                    if(gameboyContainer.bottom < buttonA.top){
                        return 0
                    }
                    return 20
                }
                anchors.left: gameboyContainer.right
                anchors.leftMargin: 20
                width: speedButtons.width

                Clickable {
                    id: stopButton
                    text: "Stop"
                    enabled: gameboy.running
                    onClicked: gameboy.stop()
                    border: 1
                    Layout.fillWidth: true
                    backgroundColor: "white"
                }
                Clickable {
                    id: resetButton
                    text: "Reset"
                    enabled: gameboy.romName !== ""
                    onClicked: gameboy.reset()
                    border: 1
                    Layout.fillWidth: true
                    backgroundColor: "white"
                }
                Clickable {
                    id: toggleButton
                    enabled: gameboy.running
                    text: gameboy.paused ? "Resume" : "Pause"
                    onClicked: gameboy.toggle()
                    border: 1
                    Layout.fillWidth: true
                    backgroundColor: "white"
                }
                Clickable {
                    id: toggleSpeedButton
                    text: "⏩"
                    enabled: gameboy.romName !== ""
                    color: gameboy.slowedDown ? "black" : "white"
                    backgroundColor: gameboy.slowedDown ? "white" : "black"
                    onClicked: gameboy.toggleSpeed()
                    border: 1
                    Layout.fillWidth: true
                }
            }
            Clickable {
                id: buttonLeft
                enabled: gameboy.running
                text: "←"
                font.pointSize: 20
                width: height
                verticalAlignment: Qt.AlignTop
                border: 1
                radius: width / 2
                anchors.bottom: buttonDown.top
                anchors.right: buttonDown.left
                backgroundColor: "white"
                onPressed: gameboy.keyDown(Qt.Key_Left)
                onReleased: gameboy.keyUp(Qt.Key_Left)
            }
            Clickable {
                id: buttonUp
                enabled: gameboy.running
                text: "↑"
                font.pointSize: 20
                width: height
                border: 1
                radius: width / 2
                anchors.bottom: buttonLeft.top
                anchors.left: buttonDown.left
                backgroundColor: "white"
                onPressed: gameboy.keyDown(Qt.Key_Up)
                onReleased: gameboy.keyUp(Qt.Key_Up)
            }
            Clickable {
                id: buttonRight
                enabled: gameboy.running
                text: "→"
                font.pointSize: 20
                width: height
                verticalAlignment: Qt.AlignTop
                border: 1
                radius: width / 2
                anchors.bottom: buttonDown.top
                anchors.left: buttonDown.right
                backgroundColor: "white"
                onPressed: gameboy.keyDown(Qt.Key_Right)
                onReleased: gameboy.keyUp(Qt.Key_Right)
            }
            Clickable {
                id: buttonDown
                enabled: gameboy.running
                text: "↓"
                font.pointSize: 20
                width: height
                border: 1
                radius: width / 2
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 100
                anchors.left: parent.left
                backgroundColor: "white"
                anchors.leftMargin: 100 + buttonLeft.width
                onPressed: gameboy.keyDown(Qt.Key_Down)
                onReleased: gameboy.keyUp(Qt.Key_Down)
            }

            Clickable {
                id: buttonStart
                enabled: gameboy.running
                text: "start"
                font.pointSize: 15
                border: 1
                radius: 10
                anchors.bottom: buttonSelect.bottom
                anchors.left: buttonSelect.right
                anchors.leftMargin: 20
                backgroundColor: "white"
                onPressed: gameboy.keyDown(Qt.Key_Return)
                onReleased: gameboy.keyUp(Qt.Key_Return)
            }
            Clickable {
                id: buttonSelect
                enabled: gameboy.running
                text: "select"
                font.pointSize: 15
                border: 1
                radius: 10
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 50
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.horizontalCenterOffset: -(buttonSelect.width / 2) - (buttonStart.anchors.leftMargin / 2)
                backgroundColor: "white"
                onPressed: gameboy.keyDown(Qt.Key_Space)
                onReleased: gameboy.keyUp(Qt.Key_Space)
            }

            Clickable {
                id: buttonB
                enabled: gameboy.running
                text: "B"
                font.pointSize: 20
                border: 1
                radius: width / 2
                width: height
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 100
                anchors.right: buttonA.left
                anchors.rightMargin: 20
                backgroundColor: "white"
                onPressed: gameboy.keyDown(Qt.Key_X)
                onReleased: gameboy.keyUp(Qt.Key_X)
            }
            Clickable {
                id: buttonA
                enabled: gameboy.running
                text: "A"
                font.pointSize: 20
                border: 1
                radius: width / 2
                width: height
                anchors.right: parent.right
                anchors.rightMargin: 100
                anchors.bottom: buttonB.top
                backgroundColor: "white"
                onPressed: gameboy.keyDown(Qt.Key_Z)
                onReleased: gameboy.keyUp(Qt.Key_Z)
            }
        },
        FilePicker {
            id: picker
            visible: stateController.state === "picker"
            anchors.fill: parent
            rootFolder: gameboy.homeFolder
            folder: gameboy.romsFolder
            onItemDoubleClicked: loadButton.clicked()
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
                        gameboy.forceActiveFocus();
                    } }
                }
            },
            Transition {
                from: "*"; to: "loading"
                SequentialAnimation {
                    ScriptAction { script: {
                        console.log("Loading display");
                    } }
                }
            },
            Transition {
                from: "*"; to: "picker"
                SequentialAnimation {
                    ScriptAction { script: {
                        if(gameboy.running && !gameboy.paused){
                            gameboy.toggle();
                        }
                        console.log("Showing file picker");
                        picker.currentIndex = -1;
                        picker.forceActiveFocus();
                    } }
                }
            }
        ]
    }
    Shortcut {
        sequence: StandardKey.Quit
        context: Qt.ApplicationShortcut
        autoRepeat: false
        onActivated: {
            console.log("Quitting");
            Qt.quit();
        }
    }
    Shortcut {
        enabled: stateController.state !== "loading"
        sequences: [StandardKey.Cancel, Qt.Key_Backspace]
        autoRepeat: false
        onActivated: backButton.clicked()
    }
    Shortcut {
        enabled: stateController.state === "loaded"
        sequences: [StandardKey.Open]
        autoRepeat: false
        onActivated: stateController.state = "picker"
    }
    Shortcut {
        enabled: stateController.state === "loaded"
        sequences: [StandardKey.Refresh]
        autoRepeat: false
        onActivated: gameboy.reset()
    }
}
