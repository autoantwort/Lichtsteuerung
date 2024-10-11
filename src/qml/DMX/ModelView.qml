import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import custom.licht 1.0
import QtQuick.Controls.Material 2.3
import "../components"

GridLayout{
    anchors.leftMargin: 5
    columnSpacing: 5
    rowSpacing: 5
    columns: 4
    rows: 4
    id: root
    signal addClicked()
    signal removeClicked(var remove)
    property alias model : sortedView.sourceModel
    property var sortModel
    property alias currentItem : listView.currentItem
    property var currentModelData : listView.currentItem ? listView.currentItem.modelItemData : null
    property alias addButton: buttonAdd
    property alias removeButton: buttonRemove
    property alias addButtonEnabled: buttonAdd.enabled
    property alias removeButtonEnabled: buttonRemove.enabled
    property alias nameInputEnabled: textName.enabled
    property alias descriptionInputEnabled: textDescription.enabled
    property alias listView : listView
    property var nameFunction: null
    // a filter function used for searching. The first parameter ist the search string, the second the modelData and the third the displayed string
    property var searchFilter: (modelData, text) => text.toLowerCase().indexOf(currentSearchText.toLowerCase()) !== -1
    property string searchHelpText;
    property string currentSearchText;

    ListView{
        Layout.fillHeight: true
        Layout.preferredWidth: Math.max(350,implicitWidth)
        Layout.columnSpan: 2
        Layout.rowSpan: parent.rows-1
        clip: true
        id:listView
        model: SortedModelVectorView{
            id: sortedView
        }

        delegate: ItemDelegate{
            property var modelItemData: modelData
            width: parent.width
            text: nameFunction ? nameFunction(modelData) : modelData.name +"("+modelData.description+")"
            onClicked: listView.currentIndex = index
            visible: root.searchFilter === null || root.currentSearchText.length === 0 || root.searchFilter(modelItemData, text)
            height: visible ? implicitHeight : 0
        }
        headerPositioning: ListView.OverlayHeader
        Component{
            id: header
            Pane {
                Component.onCompleted: {
                    background.radius = 0;
                }

                width: listView.width
                implicitHeight: sortRow.implicitHeight + (searchRow.implicitHeight - 4 - (36-Material.buttonHeight) * 2) * searchRow.visible + 12
                height: implicitHeight
                topPadding: 6
                bottomPadding: 6
                z: 2
                Material.elevation: 4
                RowLayout {
                    id: sortRow
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.bottom: searchRow.top
                    implicitHeight: sortCommboBox.implicitHeight
                    spacing: 8
                    Label{
                        text: "Sort by:"
                    }
                    ComboBox{
                        id: sortCommboBox
                        Layout.fillWidth: true
                        textRole: "name"
                        model: root.sortModel
                        property bool _firstTime: true
                        onCurrentIndexChanged: {
                            if(_firstTime){ // if we dont do that, we geht a weird bug that said we cant load a component (testet 5.12.3)
                                _firstTime = false;
                                return;
                            }
                            listView.model.sortPropertyName = model.get(currentIndex).sortPropertyName;
                        }
                    }
                    Button{
                        icon.source: sortedView.sortOrder === Qt.DescendingOrder ? "/icons/sort_order/sort-reverse-alphabetical-order.svg" : "/icons/sort_order/sort-by-alphabet.svg"
                        onClicked: sortedView.sortOrder = sortedView.sortOrder === Qt.DescendingOrder ? Qt.AscendingOrder : Qt.DescendingOrder;
                    }
                } // RowLayout
                RowLayout{
                    id: searchRow
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.top: sortRow.bottom
                    visible: root.filter !== null
                    spacing: 8

                    Label{
                        text: "Search:"
                    }
                    TextInputField{
                        id: searchInput
                        Layout.fillWidth: true
                        onTextChanged: currentSearchText = text
                        Keys.onEscapePressed: text = ""
                    }
                    Button{
                        Layout.preferredWidth: root.searchHelpText.length !== 0 ? implicitWidth / 2 - 4 : implicitWidth
                        flat: true
                        text: "X"
                        onClicked: searchInput.text = ""
                    }
                    Button{
                        Layout.preferredWidth: implicitWidth / 2 - 4
                        flat: true
                        text: "?"
                        visible: root.searchHelpText.length !== 0
                        ToolTip.visible: hovered
                        ToolTip.text: root.searchHelpText
                    }

                    Shortcut{
                        enabled: root.SwipeView.isCurrentItem
                        sequence: StandardKey.Find
                        onActivated: searchInput.forceActiveFocus()
                    }
                } // RowLayout
            }
        }

        header: sortModel ? header : null

        highlight: Rectangle{
            color: "blue"
            opacity: 0.7
        }

    }

    Button{
        Layout.row: parent.rows-1
        Layout.column: 0
        Layout.preferredWidth: listView.width/2
        id: buttonAdd
        text:"Add"
        font.pixelSize: 15
        onClicked: addClicked()
    }
    Button{
        Layout.row: parent.rows-1
        Layout.column: 1
        Layout.preferredWidth: listView.width/2
        id: buttonRemove
        text:"Remove"
        font.pixelSize: 15
        onClicked: askRemove.open();
    }
    AskWhenRemovePopup {
        id: askRemove
        onYesClicked: removeClicked(currentModelData)
    }


    Label{
        Layout.row: 0
        Layout.column: 2
        id:labelName
        text:"Name:"
    }
    TextInputField{
        Layout.row: 0
        Layout.column: 3
        Layout.fillWidth: true
        id:textName
        text: parent.currentModelData ? parent.currentModelData.name:""
        onTextChanged: if(parent.currentModelData) parent.currentModelData.name = text
    }
    Label{
        Layout.row: 1
        Layout.column: 2
        id:labelDescription
        text:"Description:"

    }
    TextInputField{
        Layout.row: 1
        Layout.column: 3
        Layout.fillWidth: true
        id:textDescription
        text: parent.currentModelData?parent.currentModelData.description:""
        onTextChanged: if(parent.currentModelData) parent.currentModelData.description = text
    }



}
