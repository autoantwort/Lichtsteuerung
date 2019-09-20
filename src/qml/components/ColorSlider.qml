import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12

Rectangle{
    border.color: "black"
    border.width: 1
    property alias selectorColor: selector.color
    property real value: 0
    Rectangle{
        id: selector
        x: 1 + value * (parent.width-3)
        width: 1
        color: "black"
        anchors.top: parent.top
        anchors.bottom: parent.bottom
    }
    Component.onCompleted: {
        if(gradient){
            gradient.orientation = Gradient.Horizontal;
        }
    }

    MouseArea{
        anchors.fill: parent
        onMouseXChanged: value = Math.max(0,Math.min(1,mouseX/width));
    }
}
