import QtQuick 2.10
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.0
import codes.eeems.gameboy 1.0
import "qrc:/codes.eeems.oxide"

OxideWindow {
    id: window
    objectName: "window"
    headerBackgroundColor: "white"
    backgroundColor: "white"
    color: "black"
    visible: stateController.state !== "loading"
    title: gameboy.running ? gameboy.romName : Qt.application.displayName
    property int scale: 5
    Component.onCompleted: stateController.state = "loaded"

    leftMenu: [
        Label {
            text: stateController.state === "picker" ? "⬅️" : "Exit"
            color: "black"
            topPadding: 5
            bottomPadding: 5
            leftPadding: 10
            rightPadding: 10
            MouseArea {
                id: backButton
                anchors.fill: parent
                onClicked: {
                    if(stateController.state === "picker"){
                        stateController.state = "loaded"
                        return;
                    }
                    if(stateController.state === "loaded"){
                        Qt.quit();
                        return;
                    }
                    stateController.state = "loaded";
                }
            }
        }
    ]

    centerMenu: [
        Label {
            color: "black"
            text: stateController.state === "picker" ? "Select a ROM" : window.title
            elide: Text.ElideRight
            Layout.fillWidth: true
        }
    ]

    rightMenu: [
        Label {
            text: "Open"
            color: "black"
            visible: stateController.state === "loaded"
            topPadding: 5
            bottomPadding: 5
            leftPadding: 10
            rightPadding: 10
            MouseArea {
                anchors.fill: parent
                onClicked: stateController.state = "picker"
            }
        },
        Label {
            text: picker.currentIndex > 0 && picker.get(picker.currentIndex, "fileIsDir") ? "Open" : "Load"
            color: picker.currentIndex > 0 ? "black" : "grey"
            visible: stateController.state === "picker"
            topPadding: 5
            bottomPadding: 5
            leftPadding: 10
            rightPadding: 10
            MouseArea {
                anchors.fill: parent
                enabled: picker.currentIndex > 0
                onClicked: {
                    if(picker.currentIndex == -1){
                        return;
                    }
                    if(picker.get(picker.currentIndex, "fileIsDir")){
                        picker.folder = picker.get(picker.currentIndex, "fileUrl");
                        return;
                    }
                    gameboy.loadROM(picker.get(picker.currentIndex, "filePath"));
                    stateController.state = "loaded";
                }
            }
        }
    ]

    initialItem: Item {
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
            anchors.left: parent.left
            anchors.leftMargin: 20
            width: 190
            enabled: stateController.state === "loaded"
            visible: enabled

            Clickable {
                text: "1x"
                border: 1
                Layout.fillWidth: true
                backgroundColor: "white"
                onClicked: window.scale = 1
            }
            Clickable {
                text: "2x"
                border: 1
                Layout.fillWidth: true
                backgroundColor: "white"
                onClicked: window.scale = 2
            }
            Clickable {
                text: "3x"
                border: 1
                Layout.fillWidth: true
                backgroundColor: "white"
                onClicked: window.scale = 3
            }
            Clickable {
                text: "4x"
                border: 1
                Layout.fillWidth: true
                backgroundColor: "white"
                onClicked: window.scale = 4
            }
            Clickable {
                text: "5x"
                border: 1
                Layout.fillWidth: true
                backgroundColor: "white"
                onClicked: window.scale = 5
            }
            Clickable {
                id: greyscaleButton
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
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 20
            color: "black"
            width: 160 * window.scale
            height: 144 * window.scale
            activeFocusOnTab: true
            visible: stateController.state === "loaded"
            Gameboy {
                id: gameboy
                anchors.centerIn: parent
                width: 160 * window.scale
                height: 144 * window.scale
                focus: true
                Keys.onPressed: (event)=> gameboy.keyDown(event.key)
                Keys.onReleased: (event)=> gameboy.keyUp(event.key)
            }
        }

        ColumnLayout {
            id: stateButtons
            anchors.bottom: speedButtons.bottom
            anchors.right: parent.right
            anchors.rightMargin: 20
            width: speedButtons.width
            enabled: stateController.state === "loaded"
            visible: enabled

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
                text: ">>"
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
            enabled: gameboy.running && stateController.state === "loaded"
            text: "←"
            font.pointSize: 32
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
            enabled: gameboy.running && stateController.state === "loaded"
            text: "↑"
            font.pointSize: 32
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
            enabled: gameboy.running && stateController.state === "loaded"
            text: "→"
            font.pointSize: 32
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
            enabled: gameboy.running && stateController.state === "loaded"
            text: "↓"
            font.pointSize: 32
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
            enabled: gameboy.running && stateController.state === "loaded"
            text: "start"
            font.pointSize: 24
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
            enabled: gameboy.running && stateController.state === "loaded"
            text: "select"
            font.pointSize: 24
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
            enabled: gameboy.running && stateController.state === "loaded"
            text: "B"
            font.pointSize: 32
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
            enabled: gameboy.running && stateController.state === "loaded"
            text: "A"
            font.pointSize: 32
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

        FilePicker {
            id: picker
            anchors.fill: parent
            visible: stateController.state === "picker"
            rootFolder: gameboy.homeFolder
            folder: gameboy.romsFolder
            onItemDoubleClicked: {
                if(picker.currentIndex == -1){
                    return;
                }
                if(picker.get(picker.currentIndex, "fileIsDir")){
                    picker.folder = picker.get(picker.currentIndex, "fileUrl");
                    return;
                }
                gameboy.loadROM(picker.get(picker.currentIndex, "filePath"));
                stateController.state = "loaded";
            }
        }
    }

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
                        gameboy.forceActiveFocus();
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
        onActivated: Qt.quit()
    }
    Shortcut {
        enabled: stateController.state !== "loading"
        sequences: [StandardKey.Cancel, "Backspace"]
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
    Shortcut {
        enabled: stateController.state === "loaded"
        sequences: ["Ctrl-P", "MediaTogglePlayPause"]
        autoRepeat: false
        onActivated: gameboy.toggle()
    }
    Shortcut {
        enabled: stateController.state === "loaded"
        sequences: ["Pause", "MediaPause"]
        autoRepeat: false
        onActivated:{
            if(gameboy.running && !gameboy.paused){
                gameboy.toggle();
            }
        }
    }
    Shortcut {
        enabled: stateController.state === "loaded"
        sequences: ["Play", "MediaPlay"]
        autoRepeat: false
        onActivated:{
            if(gameboy.running && gameboy.paused){
                gameboy.toggle();
            }
        }
    }
}
