import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.3 as SystemDialog
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
        RowLayout{
            id: root
            Item{
                Layout.fillWidth: true
                Layout.preferredWidth: inputSettingsPath.implicitWidth
                Layout.preferredHeight: inputSettingsPath.implicitHeight
                Layout.topMargin: 4
                clip: true
                id: wrapper
                TextInputField{
                    id: inputSettingsPath
                    readOnly: true
                    anchors.fill: parent
                    anchors.bottomMargin: 2
                    text: Settings.jsonSettingsFilePath
                }
            }
            Button{
                Layout.minimumWidth: implicitWidth
                Layout.leftMargin: 5
                Layout.preferredHeight: implicitHeight - 15
                text: "Save as"
                onClicked: {
                    fileDialog.openAt(Settings.jsonSettingsFilePath, false, false);
                    fileDialog.callback = function(file){
                        Settings.setJsonSettingsFilePath(file, false);
                    };
                }
            }
            Button{
                Layout.minimumWidth: implicitWidth
                Layout.leftMargin: 5
                Layout.preferredHeight: implicitHeight - 15
                text: "Load from"
                onClicked: {
                    fileDialog.openAt(Settings.jsonSettingsFilePath, false);
                    fileDialog.callback = function(file){
                        if(Settings.setJsonSettingsFilePath(file, true)){
                            popupChangedSettingsFile.visible = true;
                        }else{
                            ErrorNotifier.errorMessage += "You can not load the current opened settings file.";
                        }
                    };
                }
            }
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
    SystemDialog.FileDialog{
        property var callback;
        selectExisting: false
        defaultSuffix: ".json"
        function openAt(path, isFolder, selectExisting_ = true){
            selectFolder = isFolder;
            folder = pathToUrl(path);
            selectExisting = selectExisting_;
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
    Dialog{
        id: popupChangedSettingsFile
        modal: true
        margins: 50
        x: (parent.width - width) / 2
        y: 0
        closePolicy: Popup.NoAutoClose
        title: "Do you want to restart the light control now to load the settings file?"
        standardButtons: Dialog.No | Dialog.Yes
        onAccepted: Qt.quit();
    }

}
