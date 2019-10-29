
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import custom.licht 1.0
import QtQuick.Controls.Material 2.12
import "ControlPane"
import "components"
import "HelpSystem"
import "LedVisualisation"
import "Modules"
import "DMX"
import "Login"

ApplicationWindow {
    visible: true
    width: 900
    height: 480
    title: qsTr("Lichtsteuerung")

    Material.background: Settings.backgroundColor
    Material.foreground: Settings.foregroundColor
    Material.accent: Settings.accentColor
    Material.theme: Settings.theme

    Item{
        anchors.fill: parent
        id: root
        Shortcut{
            sequence: StandardKey.Save
            autoRepeat: false
            onActivated: {
                ModelManager.save()
                root.ToolTip.show("Data Saved",1500)
            }
        }

        ToolTip.visible: updater.progress >= 0 && updater.progress <= 100 || updater.state === UpdaterState.DownloadUpdateFailed
        ToolTip.delay: 0
        ToolTip.timeout: 20000
        function getText(state, progress){
            // enum UpdaterState {IDE_ENV, NotChecked, NoUpdateAvailible, UpdateAvailible, DownloadingUpdate, UnzippingUpdate, UnzippingFailed, PreparationForInstallationFailed, ReadyToInstall, DownloadUpdateFailed};
            switch(state){
            case UpdaterState.IDE_ENV:
                return "You are running the light control in an IDE, make no update";
            case UpdaterState.NotChecked:
            case UpdaterState.NoUpdateAvailible:
                return "";
            case UpdaterState.UpdateAvailible:
                return "Update availible";
            case UpdaterState.DownloadingUpdate:
                return "Downloading update. Progress : " + updater.progress + "%";
            case UpdaterState.UnzippingUpdate:
                return "Unzipping update ...";
            case UpdaterState.ReadyToInstall:
                return "Close this application to install the update";
            case UpdaterState.DownloadUpdateFailed:
                return "Error while downloading the update";
            case UpdaterState.UnzippingFailed:
                return "Error while unzipping the update";
            case UpdaterState.PreparationForInstallationFailed:
                return "Error while preparation for the installation of the update";
            }
            return "Unknown update state";
        }
        ToolTip.text: getText(updater.state, updater.progress)

        Rectangle{
            anchors.fill: parent
            color: Material.background
        }

        VerticalTabBar{

            id: tabBar
            currentIndex: swipeView.currentIndex

            height: parent.height
            width: 150
            x:0
            y:0
            z: 2

            VerticalTabButton {
                text: qsTr("Devices")
                enabled: UserManagment.currentUser.havePermission(Permission.DEVICE_TAB);
            }
            VerticalTabButton {
                text: qsTr("Device\nPrototypes")
                enabled: UserManagment.currentUser.havePermission(Permission.DEVICE_PROTOTYPE_TAB);
            }
            VerticalTabButton {
                text: qsTr("Program\nPrototypes")
                enabled: UserManagment.currentUser.havePermission(Permission.PROGRAMM_PROTOTYPE_TAY);
            }
            VerticalTabButton {
                text: qsTr("Programs")
                enabled: UserManagment.currentUser.havePermission(Permission.PROGRAMM_TAB);
            }
            VerticalTabButton {
                text: qsTr("Control Pane")
                Component.onCompleted: tabBar.setCurrentIndex(4)
            }
            VerticalTabButton {
                text: qsTr("Map View")
            }
            VerticalTabButton {
                text: qsTr("Login")
            }
            VerticalTabButton {
                text: qsTr("Settings")
            }
            VerticalTabButton {
                text: qsTr("Modules")
                enabled: UserManagment.currentUser.havePermission(Permission.MODULES_TAB);
            }
            VerticalTabButton {
                text: qsTr("Module\nPrograms")
                enabled: UserManagment.currentUser.havePermission(Permission.MODULE_PROGRAMS_TAB);
            }
            VerticalTabButton {
                text: qsTr("LED\nVisualisations")
            }
            VerticalTabButton {
                text: qsTr("Oscillogram")
            }
            VerticalTabButton {
                text: qsTr("Spectrum\nanalyzer")
            }
            VerticalTabButton {
                text: qsTr("Spectrogram")
            }
            VerticalTabButton {
                text: qsTr("Audio Events")
            }
            Help{
                helpButton.anchors.left: parent.left
                helpButton.anchors.right: undefined
                tooltipText: "Tab Explanation"
                enableAnimations: false
                HelpEntry{
                    titel: "Device Tab"
                    explanation: "Here you can manage all DMX devices, their adress, position, name, ..."
                    component: tabBar.contentChildren[0]
                    onEnter: tabBar.setCurrentIndex(0)
                    visible: UserManagment.currentUser.havePermission(Permission.DEVICE_TAB)
                }
                HelpEntry{
                    titel: "Device Prototype Tab"
                    explanation: "Here you can manage prototypes for DMX devices. A Prototype is for example a scanner or a lamp. You can specify the channels of a device prototype."
                    component: tabBar.contentChildren[1]
                    onEnter: tabBar.setCurrentIndex(1)
                    visible: UserManagment.currentUser.havePermission(Permission.DEVICE_PROTOTYPE_TAB)
                }
                HelpEntry{
                    titel: "Program Prototype Tab"
                    explanation: "Here you can create a program for a specific type of a device, like a lamp or a scanner."
                    component: tabBar.contentChildren[2]
                    onEnter: tabBar.setCurrentIndex(2)
                    visible: UserManagment.currentUser.havePermission(Permission.PROGRAMM_PROTOTYPE_TAY)
                }
                HelpEntry{
                    titel: "Program Tab"
                    explanation: "Here you can manage programs. A program is a collection of pairs of program prototypes and devices."
                    component: tabBar.contentChildren[3]
                    onEnter: tabBar.setCurrentIndex(3)
                    visible: UserManagment.currentUser.havePermission(Permission.PROGRAMM_PROTOTYPE_TAY)
                }
                HelpEntry{
                    titel: "Control Pane"
                    explanation: "Here you can control all lights and other devices."
                    component: tabBar.contentChildren[4]
                    onEnter: tabBar.setCurrentIndex(4)
                }
                HelpEntry{
                    titel: "Map View"
                    explanation: "You can see a map of the bar with all devices on the map. You can select a device and change the brightness of the selected device."
                    component: tabBar.contentChildren[5]
                    onEnter: tabBar.setCurrentIndex(5)
                }
                HelpEntry{
                    titel: "Login"
                    explanation: "Here you can log into and manage the light control users. You can change the permissions for a user and log in to spotify so that the light can be controlled by the music."
                    component: tabBar.contentChildren[6]
                    onEnter: tabBar.setCurrentIndex(6)
                }
            }
        }

        SwipeView {

            interactive: false
            //clip:true
            id: swipeView

            height: parent.height
            width: parent.width - tabBar.width
            x:tabBar.width
            y:0
            currentIndex: tabBar.currentIndex
            orientation: "Vertical"

            DeviceView{
                placeOnMapCallback: function(device){
                    mapView.toPlacedDevice = device;
                    swipeView.setCurrentIndex(mapView.SwipeView.index);
                }
            }

            DevicePrototypeView{}

            ProgrammPrototypeView{}

            ProgrammView{}

            ControlView{
                id:controlPane
            }

            BarMapView{
                id: mapView
            }
            
            LoginView{}

            SettingsView{}

            ModuleView{
                enabled: UserManagment.currentUser.havePermission(Permission.MODULES_TAB);
            }

            ModuleProgramView{
                enabled: UserManagment.currentUser.havePermission(Permission.MODULE_PROGRAMS_TAB);
            }

            LedVisualisationView{
                onMoveToOwnWindow: ledWindow.moveToWindow(SwipeView.index);
            }

            Oscillogram{
                visibleForUser: SwipeView.isCurrentItem
                lineColor: Material.foreground
            }

            FFTGraphView{
                visibleForUser: SwipeView.isCurrentItem
            }

            Colorplot{
                visibleForUser: SwipeView.isCurrentItem
                Slider{
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.topMargin: 50
                    anchors.leftMargin: 20
                    orientation: Qt.Vertical
                    from: .5
                    value: 1
                    to: 2
                    onValueChanged: parent.zoom = value
                }
            }

            AudioEventsView{
                visibleForUser: SwipeView.isCurrentItem
            }
        }
    }

    LedWindow{
        color: Material.background
        Material.background: Settings.backgroundColor
        Material.foreground: Settings.foregroundColor
        Material.accent: Settings.accentColor
        Material.theme: Settings.theme
        id: ledWindow
        property int insertAtIndex
        function moveToWindow(index){
            let wasNeverVisible = x === 0;
            const globalPos = swipeView.itemAt(index).mapToGlobal(0,0);
            if(wasNeverVisible) x = globalPos.x
            view = swipeView.takeItem(index);
            if(wasNeverVisible){
                width = view.width;
                y = globalPos.y + view.visibleChildren[0].height + 15 /*margin + spacing*/;
            }
            view.inOwnWindow = true;
            if(wasNeverVisible) height = Math.min(view.height, view.implicitHeight);
            visible = true;
            insertAtIndex = index;
            button = tabBar.takeItem(index);
        }
        onClosing: {
            view.inOwnWindow = false;
            swipeView.insertItem(insertAtIndex,view);
            tabBar.insertItem(insertAtIndex, button);
        }

    }

    Dialog{
        modal: true
        title: "Error"
        standardButtons: Dialog.Ok
        Overlay.modal: ModalPopupBackground{}
        width: 600
        y: 50
        x: (root.width-width)/2;
        contentItem: Label{
            text: ErrorNotifier.errorMessage
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
        }
        onClosed: if(result === Dialog.Accepted) ErrorNotifier.errorMessage = "";

        visible: ErrorNotifier.errorMessage.length !== 0
    }

    Popup {
       Overlay.modal: ModalPopupBackground{}
       closePolicy: Popup.NoAutoClose
       y: 70
       x: (parent.width - width) / 2
       modal: true
       visible: SettingsFile.status === SettingsFileStatus.LoadingFailed
       onVisibleChanged: {
           if (visible) {
               fileDialogLoader.source = "components/SystemFileDialog.qml"
           }
       }

       contentItem: ColumnLayout {
           Label {
               font.pointSize: 15
               font.bold: true
               text: "Failed to load settings file"
           }

           Label {
               id: fontReference
               text: SettingsFile.errorMessage
               Layout.bottomMargin: 5
               Layout.maximumWidth: 500
               wrapMode: Text.WrapAtWordBoundaryOrAnywhere
           }
           TextInput {
               Layout.fillWidth: true
               Layout.maximumWidth: 500
               Layout.leftMargin: 1
               Layout.rightMargin: 2
               clip: true
               color: Material.foreground
               selectionColor: Material.accentColor
               font: fontReference.font
               text: Settings.jsonSettingsFilePath
               selectByMouse: true
               readOnly: true
           }
           GridLayout {
               columns: 2
               rowSpacing: -5
               Button {
                   text: "Open file in default editor"
                   Layout.fillWidth: true
                   flat: true
                   onClicked: SettingsFile.openFileInDefaultEditor();
               }
               Button {
                   text: "Open folder where file is"
                   Layout.fillWidth: true
                   flat: true
                   onClicked: SettingsFile.openFileExplorerAtFile();
               }
               Button {
                   text: "Reload selected settings file"
                   Layout.fillWidth: true
                   onClicked: SettingsFile.reload();
               }
               Button {
                   text: "Load an other settings file"
                   Layout.fillWidth: true
                   onClicked: fileDialogLoader.item.openAt(Settings.jsonSettingsFilePath, false)
               }
           }
       }
       Loader {
           asynchronous: true
           id: fileDialogLoader
           onStatusChanged: {
               if (status === Loader.Ready) {
                   item.callback = path => {
                       SettingsFile.loadFile(path);
                   };
               }
           }
       }
    }
}
