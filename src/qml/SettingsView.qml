import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.3
import QtQuick.Window 2.12
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
            text: "Audio Capture Device:"
        }
        ComboBox{
            model: AudioManager.captureDeviceNames
            Layout.fillWidth: true
            onActivated: AudioManager.currentCaptureDevice = index
            currentIndex: AudioManager.currentCaptureDevice
            onDownChanged: if(down)AudioManager.updateCaptureDeviceList()
        }
        Label{
            Layout.fillWidth: true
            text: "Theme:"
        }
        Button{
            text: "Modify Theme and appearance"
            onClicked: modifyThemeWindow.show()
        }
    }
    FileDialog{
        property var callback;
        function openAt(path, isFolder){
            selectFolder = isFolder;
            folder = pathToUrl(path);
            open();
        }
        id: fileDialog
        title: "Please choose a file"
        onAccepted: {
            if(callback){
                callback(urlToPath(fileDialog.fileUrl));
            }else{
                console.error("Error in File Dialog in SettingsView: No callback provided!")
            }
        }
    }
    Window{
        id: modifyThemeWindow
        flags: Qt.WindowStaysOnTopHint | Qt.Dialog | Qt.WindowCloseButtonHint | Qt.WindowTitleHint
        title: "Modify Theme"
        color: pane.Material.background
        width: 350
        height: 350
        Material.theme: Settings.theme
        ModifyThemePane{
            id: pane
            anchors.fill: parent
        }
    }
}
