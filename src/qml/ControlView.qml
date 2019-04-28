import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.2
import custom.licht 1.0


ControlPanel{

    //we can only define enums here
    enum Select{
        ProgramBlock,
        Program
    }

    onExitMenuArea: programm.width = programm.height
    menuHeight: height-programBlock.y
    menuWidth: width-programm.x
    RoundButton{

        onClicked: selectProgramDialog.createProgram()
        visible: UserManagment.currentUser.havePermission(Permission.ADD_CONTROL_ITEM);
        id:programm
        text: "+"
        z:2
        anchors.margins: 5
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        onHoveredChanged: if(hovered)width=150;
        Behavior on width {
            NumberAnimation{
                duration: 150
                easing.type: Easing.OutQuad
                onRunningChanged: {
                    if(running)
                        textAnimation.start();
                    if(running&&!programm.hovered){
                        textAnimation.newText="Add Programm";
                        dimmerGroup.y = programm.y;
                        switchGroup.y = programm.y;
                        programBlock.y = programm.y;
                    }else if(!running&&programm.hovered){
                        textAnimation.newText="+";
                        dimmerGroup.y = programm.y-programm.height;
                        switchGroup.y = programm.y-programm.height*2;
                        switchGroup.y = programm.y-programm.height*2;
                        programBlock.y = programm.y-programm.height*3;
                    }
                }
            }
        }
        onYChanged: {
            if(width!=height){
                dimmerGroup.y = programm.y-programm.height
                switchGroup.y = programm.y-programm.height*2;
                programBlock.y = programm.y-programm.height*3;
            }else{
                dimmerGroup.y = programm.y;
                switchGroup.y = programm.y;
                programBlock.y = programm.y;
            }
        }

        SequentialAnimation {
            id:textAnimation
            property alias newText: setText.value
            NumberAnimation{
                target: programm.contentItem
                property: "opacity"
                duration: 60
                from: 1
                to: 0
                easing.type: Easing.Linear
            }

            PauseAnimation {
                duration: 30
            }
            PropertyAction {
                id:setText
                target: programm;
                property: "text"
            }
            NumberAnimation{
                target: programm.contentItem
                property: "opacity"
                duration: 60
                from: 0
                to: 1
                easing.type: Easing.Linear
            }
        }
    }

    RoundButton{
        visible: UserManagment.currentUser.havePermission(Permission.ADD_CONTROL_ITEM);
        z:1.3
        id: switchGroup
        text:"Switchgroup"
        anchors.right: programm.right
        anchors.left: programm.left
        Behavior on y{
            NumberAnimation{duration: 100}
        }
        onClicked: addSwitchGroupControl()
    }
    RoundButton{
        visible: UserManagment.currentUser.havePermission(Permission.ADD_CONTROL_ITEM);
        z:1.3
        id: dimmerGroup
        text:"Dimmergroup"
        anchors.right: programm.right
        anchors.left: programm.left
        Behavior on y{
            NumberAnimation{duration: 100}
        }
        onClicked: addDimmerGroupControl()
    }
    RoundButton{
        visible: UserManagment.currentUser.havePermission(Permission.ADD_CONTROL_ITEM);
        z:1.3
        id: programBlock
        text:"ProgramBlock"
        anchors.right: programm.right
        anchors.left: programm.left
        Behavior on y{
            NumberAnimation{duration: 100}
        }
        onClicked: selectProgramDialog.createProgramBlock()
    }

    Dialog{
        modality: Qt.WindowModal
        id:selectProgramDialog
        property int select: ControlView.Select.Program
        title: select === ControlView.Select.Program?"Select Program":"Select ProgramBlock"
        width:300
        function createProgram(){
            select = ControlView.Select.Program;
            visible = true;
        }
        function createProgramBlock(){
            select = ControlView.Select.ProgramBlock;
            visible = true;
        }
        contentItem: RowLayout {
            Pane{
                Layout.fillWidth: true
                ColumnLayout{
                    anchors.left: parent.left
                    anchors.right: parent.right
                    spacing: 10
                    ComboBox{
                        Layout.fillWidth: true
                        id:programmSelect
                        model: selectProgramDialog.select === ControlView.Select.Program?programmModel:programBlocksModel
                        textRole: "display"
                    }
                    RowLayout{
                        Button{
                            Layout.fillWidth: true
                            text:"Abbrechen"
                            onClicked: selectProgramDialog.visible = false
                        }
                        Button{
                            Layout.fillWidth: true
                            text:"Hinzufügen"
                            onClicked: {
                                selectProgramDialog.visible = false;
                                if(selectProgramDialog.select === ControlView.Select.Program){
                                    addProgrammControl(programmModel.data(programmModel.index(programmSelect.currentIndex,0),-1));
                                }else{
                                    addProgramBlockControl(programBlocksModel.data(programBlocksModel.index(programmSelect.currentIndex,0),-1));
                                }

                            }
                        }
                    }
                }
            }
        }
    }


}
