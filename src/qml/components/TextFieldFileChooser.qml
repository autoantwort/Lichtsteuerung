import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import "../"

RowLayout{
    id: root
    property string path
    property bool folder: false
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
            fileDialog.openAt(parent.path, parent.folder);
            fileDialog.callback = function(file){
                root.path = file;
            };
        }
    }
}
