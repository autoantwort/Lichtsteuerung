
import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import custom.licht 1.0
import QtQuick.Dialogs 1.2
import Qt.labs.handlers 1.0
ApplicationWindow {
    visible: true
    width: 900
    height: 480
    title: qsTr("Lichtsteuerung")



    header:TabBar {

        id: tabBar
        currentIndex: swipeView.currentIndex


        TabButton {
            text: qsTr("Devices")
            enabled: UserManagment.currentUser.havePermission(Permission.DEVICE_TAB);
        }
        TabButton {
            text: qsTr("DevicePrototypes")
            enabled: UserManagment.currentUser.havePermission(Permission.DEVICE_PROTOTYPE_TAB);
        }
        TabButton {
            text: qsTr("ProgrammPrototypes")
            enabled: UserManagment.currentUser.havePermission(Permission.PROGRAMM_PROTOTYPE_TAY);
        }
        TabButton {
            text: qsTr("Programms")
            enabled: UserManagment.currentUser.havePermission(Permission.PROGRAMM_TAB);
        }
        TabButton {
            text: qsTr("Control Pane")
        }
        TabButton {
            text: qsTr("Map View")
        }
        TabButton {
            text: qsTr("Login")
            Component.onCompleted: tabBar.currentIndex = 6
        }
        TabButton {
            text: qsTr("Settings")
            enabled: UserManagment.currentUser.havePermission(Permission.SETTINGS_TAB);
        }
        TabButton {
            text: qsTr("Graph")
        }
        TabButton {
            text: qsTr("Oscillogram")
        }
        TabButton {
            text: qsTr("Colorplot")
        }
    }

    SwipeView {
        interactive: false
        clip:true
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex        
        DeviceView{}

        DevicePrototypeView{}

        ProgrammPrototypeView{}

        ProgrammView{}

        ControlView{
            id:controlPane
        }

        Item{
            clip: true
            MapView{
                width:400
                height: 400
                DragHandler{}
                PinchHandler{}
            }
        }
        LoginView{}

        SettingsView{}

        Graph{
        }

        Oscillogram{
        }

        Colorplot{}
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
