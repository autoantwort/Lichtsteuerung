import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import "../"

RowLayout{
    id: root
    property string path
    property var fileChooser: null
    height: 20

    TextInputField{
        Layout.fillWidth: true
        id: textField
        text: root.path
        onAccepted: {
            root.path = text
        }
    }
    Button{
        Layout.minimumWidth: implicitWidth
        Layout.leftMargin: 5
        Layout.preferredHeight: implicitHeight - 15
        text: "File Chooser"
        onClicked: {
            if(root.fileChooser === null){
                throw "fileChooser ist not set!";
            }
            const fileDialog = root.fileChooser;
            fileDialog.openAt(parent.path);
            fileDialog.callback = function(file){
                root.path = file;
            };
        }
    }
}
