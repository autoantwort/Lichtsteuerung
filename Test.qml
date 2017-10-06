import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex
        Page{
        Rectangle{
            height: 100
            width: 100
            anchors.centerIn: parent
            color: "red"
        }
        }

        Page {
            Label {
                text: qsTr("Second page")
                anchors.centerIn: parent
            }
            Button{
                contentItem: Text {
                    id: testwe
                    text: qsTr("text");
                    rotation: 90
                }
            }
        }
    }




    footer:TabBar {

        id: tabBar
        currentIndex: swipeView.currentIndex


        TabButton {
            text: qsTr("First")
        }
        TabButton {
            text: qsTr("Second")
        }
    }
}
