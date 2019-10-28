import QtQuick 2.12
import QtQuick.Controls.Material 2.12

Rectangle{
    id:underline
    property real extendetWidth: 10
    //property real clickX: 0
    anchors.top:parent.bottom
    anchors.left:parent.left
    width: parent.contentWidth+extendetWidth
    height: 2
    radius: 1
    color: parent.enabled ? Material.accentColor : Material.hintTextColor
    //property bool hasFocus
    Behavior on width {
        NumberAnimation { easing.type: Easing.OutExpo; easing.amplitude: 5.0; easing.period: 2.0; duration: 800 }

    }/*
    Rectangle{
        id:overlay
        height: 2
        radius: 1
        color: "lightgreen"
        onXChanged: ani.start()
        NumberAnimation on x{
            id:ani
            duration: 800
            easing.type: Easing.OutExpo
            to: 0
            alwaysRunToEnd: true
        }
        opacity: parent.hasFocus?1:0
        Behavior on opacity{
            NumberAnimation{duration:opacity==0?0:100}

        }
    }
    NumberAnimation {
        id:widthAnimation
        target: overlay
        property: "width"
        duration: 800
        easing.type: Easing.OutExpo
        from:0
        to:underline.width
    }
    onClickXChanged: function(){
        if(!hasFocus){
            overlay.x=clickX
            widthAnimation.start();
        }
    }
    onHasFocusChanged: console.log("focus change")
    */
}
