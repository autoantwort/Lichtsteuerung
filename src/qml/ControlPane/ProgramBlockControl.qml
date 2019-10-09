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

            Button {
                padding: 3
                Layout.fillHeight: true
                Layout.fillWidth: true
                implicitWidth: 50
                id: startStop
                icon.source: controlData.programBlock.status === 1 ? "qrc:icons/stop.svg" : controlData.programBlock.status === 0 ? "qrc:icons/play.svg":"qrc:icons/replay.svg"
                icon.width: width
                icon.height: width
                background: null
                onClicked: {
                    if(controlData.programBlock.status === 0){
                        controlData.programBlock.start();
                    } else if(controlData.programBlock.status === 1){
                        controlData.programBlock.stop();
                    } else if(controlData.programBlock.status === 2){
                        controlData.programBlock.restart();
                    }
                }
                ToolTip.visible: hovered
                ToolTip.delay: 1000
                ToolTip.text: controlData.programBlock.status === 1 ? "Stop" : controlData.programBlock.status === 0 ? "Play":"Replay"
            }
            Button {
                padding: 1
                implicitWidth: 50
                Layout.fillHeight: true
                Layout.fillWidth: true
                id: pauseResume
                icon.source: controlData.programBlock.status === 1 ? "qrc:icons/pause.svg" : "qrc:icons/resume.png"
                icon.width: width
                icon.height: width
                background: null
                visible: controlData.programBlock.status !== 0;
                onClicked: {
                    if(controlData.programBlock.status === 1){
                        controlData.programBlock.pause();
                    } else if(controlData.programBlock.status === 2){
                        controlData.programBlock.resume();
                    }
                }
                ToolTip.visible: hovered
                ToolTip.delay: 1000
                ToolTip.text: controlData.programBlock.status === 1 ? "Pause" : "Resume"
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
