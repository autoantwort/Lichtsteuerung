import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.12

Rectangle {
    id:root
    color: "#1db954" //spotify green
    property alias coverImageSource: image.source
    property var progress: null
    property alias text: label.text
    Image {
        id: image
        fillMode: Image.PreserveAspectFit
        anchors.margins: 5
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: label.left
    }
    Label{
        id: label
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 5
    }

    Rectangle{
        visible: root.progress !== null
        height: 3
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        color: "grey"
        Rectangle{
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: parent.width * root.progress
            color: "white"
            Behavior on width {
                NumberAnimation { easing.type: Easing.Linear; duration: 1000 }

            }
        }
    }
}
