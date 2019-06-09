import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import "components"

GridLayout{
    anchors.leftMargin: 5
    columnSpacing: 5
    rowSpacing: 5
    columns: 4
    rows: 4
    signal addClicked()
    signal removeClicked(var remove)
    property alias model : listView.model
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


    ListView{
        Layout.fillHeight: true
        Layout.preferredWidth: Math.max(350,implicitWidth)
        Layout.columnSpan: 2
        Layout.rowSpan: parent.rows-1
        id:listView
        delegate: ItemDelegate{
            property var modelItemData: modelData
            width: parent.width
            text: nameFunction ? nameFunction(modelData) : modelData.name +"("+modelData.description+")"
            onClicked: listView.currentIndex = index

        }
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
        onClicked: removeClicked(currentModelData)
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
        text: listView.currentItem?currentModelData.name:""
        onTextChanged: currentModelData.name = text
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
        text: listView.currentItem?currentModelData.description:""
        onTextChanged: currentModelData.description = text
    }



}
