import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtGraphicalEffects 1.0
import custom.licht 1.0

ControlItem{
    id:item
    blockWidth: 3
    blockHeight: 1
    pressed: controlData.programBlock.status === 1
    Component.onCompleted: {
        controlData.programBlockChanged.connect(function(){
            item.parent = null;
            delete item;
        });
    }

    ControlItemBlock{
        RowLayout{
            anchors.fill: parent
            spacing: 0

            Image {
                Layout.fillHeight: true
                Layout.fillWidth: true
                fillMode: Image.PreserveAspectFit
                id: startStop
                source: controlData.programBlock.status === 1 ? "qrc:icons/stop.svg" : controlData.programBlock.status === 0 ? "qrc:icons/play.svg":"qrc:icons/replay.svg"
                MouseArea{
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        if(controlData.programBlock.status === 0){
                            controlData.programBlock.start();
                        } else if(controlData.programBlock.status === 1){
                            controlData.programBlock.stop();
                        } else if(controlData.programBlock.status === 2){
                            controlData.programBlock.restart();
                        }
                    }
                    ToolTip.visible: containsMouse
                    ToolTip.delay: 1000
                    ToolTip.text: controlData.programBlock.status === 1 ? "Stop" : controlData.programBlock.status === 0 ? "Play":"Replay"
                }
            }
            Image {
                Layout.fillHeight: true
                Layout.fillWidth: true
                fillMode: Image.PreserveAspectFit
                id: pauseResume
                source: controlData.programBlock.status === 1 ? "qrc:icons/pause.svg" : "qrc:icons/resume.png"
                visible: controlData.programBlock.status !== 0;
                MouseArea{
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        if(controlData.programBlock.status === 1){
                            controlData.programBlock.pause();
                        } else if(controlData.programBlock.status === 2){
                            controlData.programBlock.resume();
                        }
                    }
                    ToolTip.visible: containsMouse
                    ToolTip.delay: 1000
                    ToolTip.text: controlData.programBlock.status === 1 ? "Pause" : "Resume"
                }
            }
        }
    }

    ControlItemBlock{
        blockWidth: 2
        Label{
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: controlData.programBlock.name
        }
    }
}
