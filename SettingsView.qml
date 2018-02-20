import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
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
                    fileDialog.folder = Settings.jsonSettingsFilePath;
                    fileDialog.open();
                    fileDialog.addSelection(Settings.jsonSettingsFilePath);
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
                }
            }
        }
    }
    FileDialog{
        id: fileDialog
              title: "Please choose a file"
              onAccepted: {

              }
    }
}
