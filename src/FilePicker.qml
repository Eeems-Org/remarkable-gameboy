import QtQuick 2.10
import Qt.labs.folderlistmodel 2.15
import "."

Item {
    id: root
    property var nameFilters: ["*.gb"]
    property string rootFolder: "file:///home/root"
    property string folder: "file:///home/root/roms"
    property int selectedIndex: -1
    function selected(){ return folderModel.get(selectedIndex, "filePath") }

    Rectangle {
        anchors.fill: parent
        color: "white"
    }

    FolderListModel {
        id: folderModel
        nameFilters: root.nameFilters
        rootFolder: root.rootFolder
        folder: root.folder
    }

    Component {
        id: fileDelegate
        Clickable {
            text: fileName
            width: parent.width
            font.pointSize: 20
            function index(){ return folderModel.indexOf(fileUrl) }
            color: root.selectedIndex === index() ? "white" : "black"
            backgroundColor: root.selectedIndex === index() ? "black" : "white"
            onClicked: root.selectedIndex = root.selectedIndex === index() ? -1 : index()
        }
    }

    ListView {
        id: list
        anchors.fill: parent
        model: folderModel
        delegate: fileDelegate
    }
}

