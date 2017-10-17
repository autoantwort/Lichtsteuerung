import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

ModelView{
    model:deviceModel
    rows: 6
    Text{
        Layout.row: 2
        Layout.column: 2
        text:"startDMXChannel:"
        Layout.rightMargin: 20
        font.pixelSize: 15
    }
    TextInputField{
        Layout.row: 2
        Layout.column: 3
        text: parent.currentItem.data.startDMXChannel
        validator: IntValidator{
            bottom: 0
            top:512
        }
        onTextChanged: parent.currentItem.data.startDMXChannel = text.length?text:0
    }
    Text{
        Layout.row: 3
        Layout.column: 2
        text:"DevicePrototype:"
        Layout.rightMargin: 20
        font.pixelSize: 15
    }
    Text{
        Layout.row: 3
        Layout.column: 3
        text:parent.currentItem.data.prototype.name
        font.pixelSize: 15
    }
}
