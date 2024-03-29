import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import "../components"


ModelView{
    id:modelView
    model:devicePrototypeModel

    addButton.text: "Add Prototype"
    removeButton.text: "Remove Prototype"
    sortModel: ListModel{
        ListElement{
            name: "Creation Date"
            sortPropertyName: ""
        }
        ListElement{
            name: "Name"
            sortPropertyName: "name"
        }
    }

    ListView{
        clip:true
        Layout.column: 2
        Layout.columnSpan: 2
        Layout.row: 2
        Layout.fillHeight: true
        Layout.fillWidth: true
        id : channelView
        delegate: ItemDelegate{
            width: parent.width
            text: index + " :"
            onClicked: channelView.currentIndex = index
            TextInputField{
                x:parent.implicitWidth-15
                anchors.verticalCenter: parent.verticalCenter
                visible: true
                text : modelData.name
                underlineColor: parent.ListView.isCurrentItem?"lightgreen":"lightgrey"
                onCursorPositionChanged: {
                    //if(focus)
                        channelView.currentIndex = index;
                    //focus = true;*/
                }
                onTextChanged: modelData.name = text
            }
        }
        model: modelView.currentModelData ? modelView.currentModelData.channel : null
    }

    onAddClicked: ModelManager.addDevicePrototype("New DevicePrototype");
    onRemoveClicked: ModelManager.removeDmxDevicePrototype(remove);

    RowLayout{
        Layout.row: 3
        Layout.column: 2
        Layout.columnSpan: 2
        Layout.fillWidth: true
        Button{
            Layout.fillWidth: true
            text:"Add Channel"
            font.pixelSize: 15
            onClicked: modelView.currentModelData.pushChannel("te34324324327st")
        }
        Button{
            Layout.fillWidth: true
            text:"Remove Channel"
            font.pixelSize: 15
            onClicked: modelView.currentModelData.popChannel()
        }
    }
}
