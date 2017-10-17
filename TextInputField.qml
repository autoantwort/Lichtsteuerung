import QtQuick 2.7
import QtQuick.Controls 2.0


TextInput{
    selectByMouse: true
    font.pixelSize: 15
    selectionColor: "lightgreen"
    cursorDelegate: Rectangle{
        color: "lightgreen"
        x:parent.cursorRectangle.x
        y:parent.cursorRectangle.y
        width:parent.cursorRectangle.width+1
        height:parent.cursorRectangle.height
        visible: parent.cursorVisible
        Behavior on x {
            NumberAnimation { easing.type: Easing.OutCubic; easing.amplitude: 9; easing.period: 50.0; duration: 500 }
        }
    }
    Rectangle{
        anchors.top:parent.bottom
        anchors.left:parent.left
        width: parent.contentWidth+10
        height: 2
        radius: 1
        color: "lightgreen"
        Behavior on width {
            NumberAnimation { easing.type: Easing.OutExpo; easing.amplitude: 5.0; easing.period: 2.0; duration: 800 }
        }

    }
}
