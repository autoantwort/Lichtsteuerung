import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.12
import custom.licht 1.0

ControlItem{
    id:item
    blockWidth: 3
    blockHeight: 1
    ControlItemBlock{
        Button {
            padding: 1
            implicitWidth: 50
            anchors.fill: parent
            id: play
            icon.source: "/icons/play.svg"
            icon.width: width - 4
            icon.height: height - 4
            background: null
            Behavior on opacity {
                NumberAnimation{
                    duration: 100
                }
            }
        }
        Button {
            padding: 1
            implicitWidth: 50
            anchors.fill:parent
            id: pause
            icon.source: "/icons/pause.svg"
            icon.width: width - 4
            icon.height: height - 4
            background: null
            opacity: 1-play.opacity
            onClicked: {
                controlData.programm.running = !controlData.programm.running;
                item.pressed = controlData.programm.running;
                play.opacity = !controlData.programm.running;
            }
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
                        color: Material.foreground
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
