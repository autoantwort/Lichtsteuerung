
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import custom.licht 1.0
import "ControlPane"
import "components"
import "HelpSystem"
import "LedVisualisation"

ApplicationWindow {
    visible: true
    width: 900
    height: 480
    title: qsTr("Lichtsteuerung")



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
                enabled: UserManagment.currentUser.havePermission(Permission.SETTINGS_TAB);
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
                text: qsTr("Graph")
            }
            VerticalTabButton {
                text: qsTr("Oscillogram")
            }
            VerticalTabButton {
                text: qsTr("Colorplot")
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
                HelpEntry{
                    titel: "Graph"
                    explanation: "The graph shows a spectrum analysis of the currently played music (the whole windows output is captured)."
                    component: tabBar.contentChildren[10]
                    onEnter: tabBar.setCurrentIndex(10)
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

            FFTGraphView{}

            Oscillogram{
            }

            Page{
                contentItem: Label{
                    text: "The ColorPlot is currently not supported"
                }
            }
            AudioEventsView{
                visibleForUser: SwipeView.isCurrentItem
            }
        }
    }

    LedWindow{
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
        width: 300
        y: 50
        x: (root.width-width)/2;
        contentItem: Text{
            text: ErrorNotifier.errorMessage
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
        }
        onAccepted: ErrorNotifier.errorMessage = "";
        visible: ErrorNotifier.errorMessage.length !== 0
    }
}
