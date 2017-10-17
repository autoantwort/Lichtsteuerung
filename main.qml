import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Lichtsteuerung")


    header:TabBar {

        id: tabBar
        currentIndex: swipeView.currentIndex


        TabButton {
            text: qsTr("Devices")
        }
        TabButton {
            text: qsTr("DevicePrototypes")
        }
        TabButton {
            text: qsTr("ProgrammPrototypes")
        }
        TabButton {
            text: qsTr("Programms")
        }
    }

    SwipeView {
        clip:true
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex        
        DeviceView{}

        DevicePrototypeView{}

        ProgrammPrototypeView{}

        ProgrammView{}
    }
}
