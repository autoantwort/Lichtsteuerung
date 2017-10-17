import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

GridLayout{
    anchors.leftMargin: 5
    columnSpacing: 5
    rowSpacing: 5
    columns: 4
    rows: 4
    property alias model : listView.model
    property alias currentItem : listView.currentItem
    ListView{
        Layout.fillHeight: true
        Layout.preferredWidth: 300
        Layout.columnSpan: 2
        Layout.rowSpan: parent.rows-1
        id:listView
        delegate: ItemDelegate{
            property var data: modelData
            width: parent.width
            text: modelData.name +"("+modelData.description+")"
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
        Layout.preferredWidth: 150
        id: buttonAdd
        text:"+"
        font.pixelSize: 15
    }
    Button{
        Layout.row: parent.rows-1
        Layout.column: 1
        Layout.preferredWidth: 150
        id: buttonRemove
        text:"-"
        font.pixelSize: 15
    }


    Text{
        Layout.row: 0
        Layout.column: 2
        id:labelName
        text:"Name:"
        Layout.rightMargin: 20
        font.pixelSize: 15
    }
    TextInputField{
        Layout.row: 0
        Layout.column: 3
        Layout.fillWidth: true
        id:textName
        text: listView.currentItem.data.name
        onTextChanged: listView.currentItem.data.name = text
    }
    Text{
        Layout.row: 1
        Layout.column: 2
        id:labelDescription
        text:"Description:"
        font.pixelSize: 15

    }
    TextInputField{
        Layout.row: 1
        Layout.column: 3
        Layout.fillWidth: true
        id:textDescription
        text: listView.currentItem.data.description
        onTextChanged: listView.currentItem.data.description = text
    }



}
