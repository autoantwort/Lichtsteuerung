import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtGraphicalEffects 1.0
import custom.licht 1.0

ControlItem{
    id:item
    blockWidth: 3
    blockHeight: 1
    pressed: controlData?!controlData.programm.running:false
    onPressedChanged: {
        if(controlData.programm.running){
            play.opacity = 1;
        }else{
            play.opacity = 0;
        }
    }
    ControlItemBlock{
        Image {
            anchors.margins: 1
            anchors.fill:parent
            id: play
            source: "icons/play.svg"
            Behavior on opacity {
                NumberAnimation{
                    duration: 100
                }
            }
        }
        Image {
            anchors.margins: 1
            anchors.fill:parent
            id: pause
            source: "icons/pause.svg"
            opacity: 1-play.opacity
        }
        MouseArea{
            anchors.fill:parent

            onClicked: {controlData.programm.running=!controlData.programm.running;}

        }
    }

    ControlItemBlock{
        blockWidth: 2
        Label{
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: controlData?controlData.programm.name:"null"
        }
    }

    popup: Popup{
            id:popup
            ColumnLayout{
                Label{
                    text:"Speed:"
                }
                Slider{
                    id:speedSlider
                    from: 0.1
                    to:10
                    value: controlData?controlData.programm.speed:1
                    onValueChanged: if(controlData)controlData.programm.speed = value;
                    Text {
                        id: sliderSpeed
                        text: speedSlider.value.toFixed(1)
                        anchors.bottom: speedSlider.handle.top
                        anchors.horizontalCenter: parent.handle.horizontalCenter
                        anchors.bottomMargin: 2
                    }
                }
            }
            closePolicy: Popup.CloseOnPressOutside
        }
}
