import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import "components"

Pane{
    GridLayout{
        anchors.left: parent.left
        anchors.right: parent.right
        rowSpacing: 16
        columns: 2
        Label{
            text: "Settings file path:"
        }
        TextFieldFileChooser{
            Layout.fillWidth: true
            folder: false
            path: Settings.jsonSettingsFilePath
            onPathChanged: {Settings.jsonSettingsFilePath = path;path = Settings.jsonSettingsFilePath;}
            fileChooser: fileDialog
        }

        Label{
            Layout.fillWidth: true
            text: "Driver file path:"
        }
        TextFieldFileChooser{
            Layout.fillWidth: true
            folder: false
            path: Settings.driverFilePath
            onPathChanged: {Settings.driverFilePath = path;path = Settings.driverFilePath;}
            fileChooser: fileDialog
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
        TextFieldFileChooser{
            Layout.fillWidth: true
            folder: true
            path: Settings.moduleDirPath
            onPathChanged: {Settings.moduleDirPath = path;path = Settings.moduleDirPath;}
            fileChooser: fileDialog
        }

        Label{
            Layout.fillWidth: true
            text: "Compiler File Path:"
        }
        TextFieldFileChooser{
            Layout.fillWidth: true
            folder: true
            path: Settings.compilerPath
            onPathChanged: {Settings.compilerPath = path;path = Settings.compilerPath;}
            fileChooser: fileDialog
        }

        Label{
            Layout.fillWidth: true
            text: "Include Path:"
        }
        TextFieldFileChooser{
            Layout.fillWidth: true
            folder: true
            path: Settings.includePath
            onPathChanged: {Settings.includePath = path;path = Settings.includePath;}
            fileChooser: fileDialog
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
        TextFieldFileChooser{
            Layout.fillWidth: true
            folder: false
            path: Settings.audioCaptureFilePath
            onPathChanged: {Settings.audioCaptureFilePath = path;path = Settings.audioCaptureFilePath;}
            fileChooser: fileDialog
        }
        Label{
            Layout.fillWidth: true
            text: "Audio Capture Device:"
        }
        ComboBox{
            model: AudioManager.captureDeviceNames
            Layout.fillWidth: true
            onActivated: AudioManager.currentCaptureDevice = index
            currentIndex: AudioManager.currentCaptureDevice
            onDownChanged: if(down)AudioManager.updateCaptureDeviceList()
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
