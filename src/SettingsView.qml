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
        Label{
            text: "Update pause for dmx in ms:"
        }
        TextInputField{
            validator: IntValidator{
                bottom: 10
                top: 10000
            }
            text: Settings.updatePauseInMs;
            onAccepted: Settings.updatePauseInMs = text;
        }
        Label{
            Layout.fillWidth: true
            text: "Module Directory:"
        }
        TextInputField{
            Layout.fillWidth: true
            text: Settings.moduleDirPath
            //onAccepted: Settings.moduleDirPath = text;
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    fileDialog.selectFolder = true;
                    fileDialog.selectMultiple = false;
                    fileDialog.folder = Settings.moduleDirPath;
                    fileDialog.open();
                    fileDialog.addSelection(Settings.moduleDirPath);
                    fileDialog.callback = function(file){
                        console.log(file);
                        Settings.moduleDirPath = file;
                        parent.text = file;
                    };
                }
            }
        }
        Label{
            Layout.fillWidth: true
            text: "Compiler File Path:"
        }
        TextInputField{
            Layout.fillWidth: true
            text: Settings.compilerPath
            onAccepted: Settings.compilerPath = text;
            Button{
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.topMargin: -5
                anchors.bottomMargin: -5
                anchors.bottom: parent.bottom
                text: "File Chooser"
                onClicked: {
                    fileDialog.selectFolder = false;
                    fileDialog.selectMultiple = false;
                    fileDialog.folder = Settings.compilerPath;
                    fileDialog.open();
                    fileDialog.addSelection(Settings.compilerPath);
                    fileDialog.callback = function(file){
                        console.log(file);
                        Settings.compilerPath = file;
                    };
                }
            }
        }

        Label{
            Layout.fillWidth: true
            text: "Include Path:"
        }
        TextInputField{
            Layout.fillWidth: true
            text: Settings.includePath
            onAccepted: Settings.includePath = text;
            Button{
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.topMargin: -5
                anchors.bottomMargin: -5
                anchors.bottom: parent.bottom
                text: "File Chooser"
                onClicked: {
                    fileDialog.selectFolder = true;
                    fileDialog.selectMultiple = false;
                    fileDialog.folder = Settings.includePath;
                    fileDialog.open();
                    fileDialog.addSelection(Settings.includePath);
                    fileDialog.callback = function(file){
                        console.log(file);
                        Settings.includePath = file;
                    };
                }
            }
        }
        Label{
            Layout.fillWidth: true
            text: "Compiler Flags:"
        }
        TextInputField{
            Layout.fillWidth: true
            text: Settings.compilerFlags
            onAccepted: Settings.compilerFlags = text;
        }
        Label{
            Layout.fillWidth: true
            text: "Compiler Library Flags:"
        }
        TextInputField{
            Layout.fillWidth: true
            text: Settings.compilerLibraryFlags
            onAccepted: Settings.compilerLibraryFlags = text;
        }

        Label{
            Layout.fillWidth: true
            text: "AudioCaptureLib:"
        }
        TextInputField{
            Layout.fillWidth: true
            text: Settings.includePath
            onAccepted: Settings.includePath = text;
            Button{
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.topMargin: -5
                anchors.bottomMargin: -5
                anchors.bottom: parent.bottom
                text: "File Chooser"
                onClicked: {
                    fileDialog.selectFolder = false;
                    fileDialog.selectMultiple = false;
                    fileDialog.folder = Settings.audioCaptureFilePath;
                    fileDialog.open();
                    fileDialog.addSelection(Settings.audioCaptureFilePath);
                    fileDialog.callback = function(file){
                        Settings.audioCaptureFilePath = file;
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
