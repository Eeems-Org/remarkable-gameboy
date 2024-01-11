import QtQuick 2.10
import QtQuick.Layouts 1.0
import Qt.labs.folderlistmodel 2.15
import QtQuick.Controls 2.4
import "."

Item {
    id: root
    focus: true
    property var nameFilters: ["*.gb", "*.gbc"]
    property alias rootFolder: folderModel.rootFolder
    property alias folder: folderModel.folder
    property alias currentIndex: listView.currentIndex
    property alias currentItem: listView.currentItem
    function get(index, prop){ return folderModel.get(index, prop); }
    function forceActiveFocus(){ listView.forceActiveFocus(); }
    signal selected()
    signal itemClicked()
    signal itemDoubleClicked()

    Rectangle {
        anchors.fill: parent
        color: "white"
    }

    FolderListModel {
        id: folderModel
        nameFilters: root.nameFilters
        rootFolder: root.rootFolder
        folder: root.folder
        showDotAndDotDot: true
        showOnlyReadable: true
        showDirsFirst: true
        sortField: FolderListModel.Name
        onStatusChanged: {
            if(folderModel.status == FolderListModel.Ready){
                listView.currentIndex = 1;
            }
        }
    }

    Component {
        id: fileDelegate
        Clickable {
            visible: fileName != "."
            text: fileName !== ".." ? fileName + (fileIsDir ? "/" : "") : "(Parent Directory)"
            width: {
                if(parent == undefined){
                    return undefined;
                }
                if(scrollbar.visible){
                    return parent.width - scrollbar.width;
                }
                return parent.width;
            }
            height: this.visible ? undefined : 0
            font.pointSize: 20
            color: listView.currentItem === this ? "white" : "black"
            backgroundColor: listView.currentItem === this ? "black" : "white"
            propagateComposedEvents: true
            onClicked: {
                listView.currentIndex = index;
                root.itemClicked();
            }
            onDoubleClicked: {
                listView.currentIndex = index;
                root.itemDoubleClicked();
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        Clickable {
            id: upButton
            text: "▲"
            border: enabled ? 1 : 0
            enabled: !listView.atYBeginning
            color: enabled ? "black" : "white"
            Layout.fillWidth: true
            onClicked: {
                listView.scrollIndex = listView.scrollIndex - listView.pageSize();
                if(listView.scrollIndex < 1){
                    listView.scrollIndex = 1;
                    listView.positionViewAtBeginning();
                }else{
                    listView.positionViewAtIndex(listView.scrollIndex, ListView.Beginning);
                }
            }
        }

        ListView {
            id: listView
            property int scrollIndex: 0
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            focus: true
            activeFocusOnTab: true
            snapMode: ListView.SnapOneItem
            keyNavigationEnabled: false
            boundsBehavior: Flickable.StopAtBounds
            model: folderModel
            delegate: fileDelegate
            ScrollIndicator.vertical: ScrollIndicator {
                id: scrollbar
                width: 10
                contentItem: Rectangle {
                    color: "black"
                    implicitWidth: 6
                    implicitHeight: 100
                }
                background: Rectangle {
                    color: "white"
                    border.color: "black"
                    border.width: 1
                    implicitWidth: 6
                    implicitHeight: 100
                }
            }
            onCurrentIndexChanged: {
                forceActiveFocus();
                if(currentIndex === -1){
                    return;
                }
                if(currentIndex === 0){
                    currentIndex = 1;
                }else if(currentIndex >= count){
                    currentIndex = count - 1;
                }
                scrollIndex = currentIndex;
                positionViewAtIndex(currentIndex, ListView.Contain)
            }

            function pageHeight(){
                if(!contentItem.children.length){
                    return 1;
                }
                // Don't use [0] as that's always hidden
                var item = contentItem.children[1];
                if(!item){
                    return 1;
                }
                return height / item.height;
            }
            function pageSize(){ return Math.floor(pageHeight()); }

            Keys.onUpPressed: (event) => listView.decrementCurrentIndex()
            Keys.onDownPressed: (event) => listView.incrementCurrentIndex()
            Keys.onReturnPressed: (event) => root.itemDoubleClicked()
            Keys.onSpacePressed: (event) => root.itemDoubleClicked()
            Keys.onSelectPressed: (event) => root.itemDoubleClicked()
            Keys.onPressed: (event) => {
                if(event.key === Qt.Key_PageUp){
                    currentIndex -= listView.pageSize();
                    if(currentIndex < 1){
                        currentIndex = 1;
                    }
                    currentIndexChanged();
                }else if(event.key === Qt.Key_PageDown){
                    currentIndex += listView.pageSize();
                    currentIndexChanged();
                }
            }
        }

        Clickable {
            id: downButton
            text: "▼"
            border: enabled ? 1 : 0
            enabled: !listView.atYEnd
            color: enabled ? "black" : "white"
            Layout.fillWidth: true
            onClicked: {
                console.log(listView.pageSize());
                listView.scrollIndex = listView.scrollIndex + listView.pageSize();
                listView.positionViewAtIndex(listView.scrollIndex, ListView.Beginning);
            }
        }
    }
}

