import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.2
import custom.licht 1.0


ControlPanel{



    onExitMenuArea: programm.width = programm.height
    menuHeight: height-switchGroup.y
    menuWidth: width-programm.x
    RoundButton{

        onClicked: selectProgrammDialog.visible=true
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
                    }else if(!running&&programm.hovered){
                        textAnimation.newText="+";
                        dimmerGroup.y = programm.y-programm.height;
                        switchGroup.y = programm.y-programm.height*2;
                    }
                }
            }
        }
        onYChanged: {
            if(width!=height){
                dimmerGroup.y = programm.y-programm.height
                switchGroup.y = programm.y-programm.height*2;
            }else{
                dimmerGroup.y = programm.y;
                switchGroup.y = programm.y;
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

    Dialog{
        modality: Qt.WindowModal
        id:selectProgrammDialog
        title: "Select Programm"
        width:300
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
                        model: programmModel
                        textRole: "display"
                    }
                    RowLayout{
                        Button{
                            Layout.fillWidth: true
                            text:"Abbrechen"
                            onClicked: dialog.visible = false
                        }
                        Button{
                            Layout.fillWidth: true
                            text:"Hinzufügen"
                            onClicked: {
                                    selectProgrammDialog.visible = false;
                                    addProgrammControl(programmModel.data(programmModel.index(programmSelect.currentIndex,0),-1));

                            }
                        }
                    }
                }
            }
        }
    }


}
