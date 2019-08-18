import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.2
import custom.licht 1.0
import "../HelpSystem"


ControlPanel{

    //we can only define enums here
    enum Select{
        ProgramBlock,
        Program
    }

    id: controlPane

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
        onHoveredChanged: {
            if(hovered){
                width=150;
            }
            // if you hover only a very short time about the button, we have to redo the animation
            const newText = programm.hovered ? "Add Programm" : "+";
            if(newText !== textAnimation.newText && textAnimation.running){
                textAnimation.newText = newText;
                textAnimation.stop();
                if(programm.text !== newText){
                    textAnimation.start();
                }else{
                    programm.contentItem.opacity = 1;
                }
            }
        }

        Behavior on width {
            NumberAnimation{
                duration: 150
                easing.type: Easing.OutQuad
                onRunningChanged: {
                    if(running&&!programm.hovered){
                        dimmerGroup.y = programm.y;
                        switchGroup.y = programm.y;
                        programBlock.y = programm.y;
                    }else if(!running&&programm.hovered){
                        dimmerGroup.y = programm.y-programm.height;
                        switchGroup.y = programm.y-programm.height*2;
                        switchGroup.y = programm.y-programm.height*2;
                        programBlock.y = programm.y-programm.height*3;
                    }
                    const newText = programm.hovered ? "Add Programm" : "+";
                    if(newText !== textAnimation.newText){
                        textAnimation.newText = newText;
                        textAnimation.stop();
                        if(programm.text !== newText){
                            textAnimation.start();
                        }
                    }
                    if(running){
                        textAnimation.start();
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

    Help{
        id: help
        property SwitchGroupControl switchControl: null
        property DimmerGroupControl dimmerControl: null
        property DimmerGroupControl programControl: null
        property DimmerGroupControl control: null
        helpButton.anchors.right: programm.left
        enableAnimations: false
        tooltipText: "Control Pane Explanation"
        onStart: {
            switchControl = null;
            dimmerControl = null;
            control = null;
            //     enum Type{PROGRAMM, SWITCH_GROUP, DIMMER_GROUP, PROGRAM_BLOCK};
            for(let i = 0; i < controlPane.children.length;++i){
                if(!(controlPane.children[i] instanceof ControlItem))
                    continue;
                if(control === null)
                    control = controlPane.children[i];
                if(switchControl === null && controlPane.children[i].controlData.type === ControlType.SWITCH_GROUP){
                    switchControl = controlPane.children[i];
                }else if(dimmerControl === null && controlPane.children[i].controlData.type === ControlType.DIMMER_GROUP){
                    dimmerControl = controlPane.children[i];
                }else if(programControl === null && controlPane.children[i].controlData.type === ControlType.PROGRAMM){
                    programControl = controlPane.children[i];
                }
            }
        }
        allowEntryJumping: true

        HelpEntry{
            titel: "Move Control Items"
            explanation: "You can drag a control item to a different location"
            component: help.control
            visible: help.control !== null && UserManagment.currentUser.havePermission(Permission.MOVE_CONTROL_ITEM);
        }
        HelpEntry{
            titel: "Control Item Properties"
            explanation: "Right click a control item to open the menu of the item. (click next)"
            component: help.control
            visible: help.control !== null && (UserManagment.currentUser.havePermission(Permission.REMOVE_CONTROL_ITEM) || UserManagment.currentUser.havePermission(Permission.CHANGE_CONTROL_ITEM_VISIBILITY));
        }
        HelpEntry{
            titel: "Control Item Properties"
            explanation: "Here you can open the settings, change the visibility and delete a control item."
            component: help.control ? help.control.menu.contentItem : null
            visible: help.control !== null && (UserManagment.currentUser.havePermission(Permission.REMOVE_CONTROL_ITEM) || UserManagment.currentUser.havePermission(Permission.CHANGE_CONTROL_ITEM_VISIBILITY));
            onEnter: {
                help.control.menu.x = help.control.width/2;
                help.control.menu.y = help.control.height/2;
                help.control.menu.visible = true;
            }
            onLeave: help.control.menu.visible = false;
        }
        HelpEntry{
            titel: "Control Item Visibility"
            explanation: "Here you can change the visibility of the control item for different users. So you can hide a item for users."
            component: help.control ? help.control.visibilityPopup.contentItem : null
            visible: help.control !== null && UserManagment.currentUser.havePermission(Permission.CHANGE_CONTROL_ITEM_VISIBILITY);
            onEnter: {
                help.control.visibilityPopup.x = help.control.width/2;
                help.control.visibilityPopup.y = help.control.height/2;
                help.control.visibilityPopup.visible = true;
            }
            onLeave: help.control.visibilityPopup.visible = false;
        }

        HelpEntry{
            titel: "Switch Group"
            explanation: "With a switch group you can turn on and off multiple devices."
            component: help.switchControl
            visible: help.switchControl !== null
        }
        HelpEntry{
            titel: "Switch Group Settings"
            explanation: "Here you can change the name and the timeouts of the group. You can also select the devices that should be switched on and off."
            component: help.switchControl ? help.switchControl.popup.contentItem : null
            visible: help.switchControl !== null && (UserManagment.currentUser.havePermission(Permission.CHANGE_TIMEOUTS) || UserManagment.currentUser.havePermission(Permission.CHANGE_GROUP_NAME) || UserManagment.currentUser.havePermission(Permission.CHANGE_GROUP_DEVICES));
            onEnter: {
                help.switchControl.popup.x = help.switchControl.width/2;
                help.switchControl.popup.y = help.switchControl.height/2;
                help.switchControl.popup.visible = true;
            }
            onLeave: help.switchControl.popup.visible = false;
        }
        HelpEntry{
            titel: "Dimmer Group"
            explanation: "With a dimmer group you can change the brightness of a group of devices."
            component: help.dimmerControl
            visible: help.dimmerControl !== null
        }
        HelpEntry{
            titel: "Open Settings"
            explanation: "Hover about the top right corner of a control item and click the gear to open the settings of a control item. (click next)"
            component: help.dimmerControl
            yShift: component ? -help.dimmerControl.height/2 + 9 : 0
            visible: help.dimmerControl !== null
        }
        function enterDimmerControl(){
            help.dimmerControl.popup.x = help.dimmerControl.width/2;
            help.dimmerControl.popup.y = help.dimmerControl.height/2;
            help.dimmerControl.popup.visible = true;
            help.dimmerControl.popup.closePolicy = Popup.NoAutoClose;
        }
        function leaveDimmerControl(){
            help.dimmerControl.popup.closePolicy = Popup.CloseOnEscape | Popup.CloseOnPressOutside;
            if(currentIndex<=7||currentIndex>10)
                help.dimmerControl.popup.visible = false;
        }
        HelpEntry{
            titel: "Dimmer Min Max Mapping"
            explanation: "Here you can change the name of the dimmer group. You can also change the min max mapping of the group. A device is gets a default value and maybe a value from a program. This value is filtered by this min max mapping. The moving black number represents the given brightness of the devices and under it you can see the resulting brightness after the filtering."
            component: help.dimmerControl ? help.dimmerControl.popup.background : null
            yShift: -65
            visible: help.dimmerControl !== null && (UserManagment.currentUser.havePermission(Permission.CHANGE_MIN_MAX_MAPPING) || UserManagment.currentUser.havePermission(Permission.CHANGE_GROUP_NAME));
            onEnter: help.enterDimmerControl();
            onLeave: help.leaveDimmerControl();
        }
        HelpEntry{
            titel: "Dimmer Override Setting"
            explanation: "The value of a Device can be determined in two ways. Either a default value is set, then programs run and the result gets mapped to a different range. Alternatively, the value can simply be set to a specific value that overwrites alternative 1. 'Use value as default' describes alternative 1, 'Override with value' alternative 2."
            component: help.dimmerControl ? help.dimmerControl.popup.background : null
            yShift: 30
            visible: help.dimmerControl !== null && (UserManagment.currentUser.havePermission(Permission.CHANGE_MIN_MAX_MAPPING));
            onEnter: help.enterDimmerControl();
            onLeave: help.leaveDimmerControl();
        }
        HelpEntry{
            titel: "Dimmer Group Devices"
            explanation: "Here you can select the devices that are affected by this dimmer group."
            component: help.dimmerControl ? help.dimmerControl.popup.background : null
            yShift: 65
            visible: help.dimmerControl !== null && (UserManagment.currentUser.havePermission(Permission.CHANGE_GROUP_DEVICES));
            onEnter: help.enterDimmerControl();
            onLeave: help.leaveDimmerControl();
        }
        HelpEntry{
            titel: "Program"
            explanation: "With this item you can start and stop programs. In the settings of this control item you can change the speed of the program."
            component: help.programControl
            visible: help.dimmerControl !== null
            position: HelpEntry.Position.West
        }
    }


}
