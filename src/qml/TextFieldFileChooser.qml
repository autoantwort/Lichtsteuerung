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
    Item{
        Layout.fillWidth: true
        Layout.preferredWidth: textField.implicitWidth
        Layout.preferredHeight: textField.implicitHeight
        Layout.topMargin: 4
        id: wrapper
        clip: true
        TextInputField{
            id:textField
            text: root.path
            anchors.fill: parent
            anchors.bottomMargin: 2
            onAccepted: {
                root.path = text
            }
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
            path = root.path
            fileDialog = root.fileChooser;
            fileDialog.selectFolder = parent.folder;
            fileDialog.selectMultiple = !parent.folder;
            fileDialog.folder = "file:///"+path.substring(0,path.lastIndexOf("/"));
            fileDialog.addSelection("file:///"+path);
            fileDialog.open();
            fileDialog.callback = function(file){
                root.path = file;
            };
        }
    }
}
