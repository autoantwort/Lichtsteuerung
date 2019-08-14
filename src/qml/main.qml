
import QtQuick 2.7
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.0
import custom.licht 1.0
import "ControlPane"
import "components"

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
                text: qsTr("Graph")
            }
            VerticalTabButton {
                text: qsTr("Oscillogram")
            }
            VerticalTabButton {
                text: qsTr("Colorplot")
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

            FFTGraphView{}

            Oscillogram{
            }

            Colorplot{}
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
