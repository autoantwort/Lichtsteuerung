import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12

Rectangle{
    border.color: "black"
    border.width: 1
    property alias selectorColor: selector.color
    property real value: 0
    property int orientation: Gradient.Horizontal
    Rectangle{
        id: selector
        x: 1 + value * (parent.width-3)
        y: 1 + value * (parent.height-3)
        width: 1
        height: 1
        color: "black"
        anchors.top: orientation === Gradient.Horizontal ? parent.top : undefined
        anchors.bottom: orientation === Gradient.Horizontal ? parent.bottom : undefined
        anchors.left: orientation === Gradient.Vertical ? parent.left : undefined
        anchors.right: orientation === Gradient.Vertical ? parent.right : undefined
    }
    Component.onCompleted: {
        if(gradient){
            gradient.orientation = orientation;
        }
    }

    MouseArea{
        anchors.fill: parent
        onMouseXChanged: if(orientation === Gradient.Horizontal)value = Math.max(0,Math.min(1,mouseX/width));
        onMouseYChanged: if(orientation === Gradient.Vertical)value = Math.max(0,Math.min(1,mouseY/height));
    }
}
