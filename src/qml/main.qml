
import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import custom.licht 1.0
import QtQuick.Dialogs 1.2
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

        ToolTip.visible: updater.progress >= 0 && updater.progress <= 100
        ToolTip.delay: 0
        ToolTip.timeout: 20000
        ToolTip.text: updater.progress < 100 ? "Downloading update. Progress : " + updater.progress + "%" : "Downloading update finished. Restart to update."

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

    MessageDialog{
        text: ErrorNotifier.errorMessage
        modality: "WindowModal"
        icon: "Critical"
        title: "Error"
        standardButtons: "Ok"
        onTextChanged: visible = true;

    }
}
