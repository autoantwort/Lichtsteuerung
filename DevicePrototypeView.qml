import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0


ModelView{
    id:modelView
    model:devicePrototypeModel

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
                anchors.baseline: parent.baseline
                visible: true
                text : name
                underlineColor: parent.ListView.isCurrentItem?"lightgreen":"lightgrey"
                onCursorPositionChanged: {
                    //if(focus)
                        channelView.currentIndex = index;
                    //focus = true;*/
                }
                onTextChanged: name = text
            }
        }
        model: modelView.currentItem.data.channel
    }

    onAddClicked: ModelManager.addDevicePrototype("New DevicePrototype");

    RowLayout{
        Layout.row: 3
        Layout.column: 2
        Layout.columnSpan: 2
        Layout.fillWidth: true
        Button{
            Layout.fillWidth: true
            text:"+"
            font.pixelSize: 15
            onClicked: modelView.currentItem.data.pushChannel("te34324324327st")
        }
        Button{
            Layout.fillWidth: true
            text:"-"
            font.pixelSize: 15
            onClicked: modelView.currentItem.data.popChannel()
        }
    }
}
