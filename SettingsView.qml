import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

Pane{
    GridLayout{
        anchors.left: parent.left
        anchors.right: parent.right
        rowSpacing: 20
        columns: 2
        Label{
            text: "Settings file path:"
        }
        TextInputField{
            text: Settings.jsonSettingsFilePath
            onAccepted: Settings.jsonSettingsFilePath = text;
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    //fileDialog.folder = Settings.jsonSettingsFilePath;
                    fileDialog.open();
                    /*fileDialog.addSelection(Settings.jsonSettingsFilePath);
                    fileDialog.callback = function(file){
                        Settings.jsonSettingsFilePath = file;
                    };*/
                }
            }
        }
        Label{
            Layout.fillWidth: true
            text: "Driver file path:"
        }
        TextInputField{
            Layout.fillWidth: true
            text: Settings.driverFilePath
            onAccepted: Settings.driverFilePath = text;
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    fileDialog.folder = Settings.driverFilePath;
                    fileDialog.open();
                    fileDialog.addSelection(Settings.driverFilePath);
                    fileDialog.callback = function(file){
                        console.log(file);
                        Settings.driverFilePath = file;
                    };
                }
            }
        }
    }
    FileDialog{
        property var callback;
        id: fileDialog
        title: "Please choose a file"
        onAccepted: {
                  console.log("fftey:"+callback);
                  if(callback)callback(fileDialog.fileUrl.toString().substring(8));
              }
        onSelectionAccepted: console.log("3")
        onVisibleChanged: console.info("this")
        onRejected: console.log("rej")
        Component.onCompleted: console.log("comp")
    }
}
