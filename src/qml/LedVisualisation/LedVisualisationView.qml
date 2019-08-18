import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12

ColumnLayout{
    property bool inOwnWindow: false
    signal moveToOwnWindow;
    Label{
        visible: !inOwnWindow
        Layout.margins: 5
        Layout.fillWidth: true
        wrapMode: "WordWrap"
        text: "Here you can see a visualisation of the LED stripes used in the Program Block view. The refresh rate is 60 Hz, so the real stripe will look different with a refresh rate up to 1000 Hz."
    }

    ListView{
        Layout.margins: 5
        Layout.fillHeight: true
        Layout.fillWidth: true
        model: ledConsumer
        implicitHeight: contentHeight
        id: listView
        clip: true
        delegate:ColumnLayout{
            width: listView.width
            Label{
                Layout.fillWidth: true
                text: modelData.name + (modelData.active? " (running)" : " (not running)");
            }
            Flow{
                Layout.fillWidth: true
                Layout.bottomMargin: 10
                width: listView.width
                Repeater{
                    model: modelData
                    Rectangle{
                        width: 10
                        height: 10
                        radius: 5
                        color: ledColor
                    }
                }
            }
        }
    }
    Button{
        visible: !inOwnWindow
        Layout.margins: 5
        text: "Move into own window"
        onClicked: moveToOwnWindow();
    }
}


