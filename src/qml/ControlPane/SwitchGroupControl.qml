import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.12
import custom.licht 1.0
import ".."
import "../components"

ControlItem{
    id:item
    blockWidth: 3
    blockHeight: 1
    pressed: controlData?controlData.activated:false
    onPressedChanged: {
        if(controlData.activated){
            play.opacity = 0;
        }else{
            play.opacity = 1;
        }
        if(controlData.activated){
            disabledRectAni.duration = controlData.activateCooldown
        }else{
            disabledRectAni.duration = controlData.deactivateCooldown
        }
        disabledRectAni.start()
    }
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
                controlData.activated=!controlData.activated;
            }
        }
        Rectangle{
            id:disabledRect
            color: Material.foreground
            opacity: 0.3
            anchors.left: play.left
            anchors.right: play.right
            anchors.bottom: play.bottom
        }

        NumberAnimation {
            id: disabledRectAni
            target: disabledRect
            property: "height"
            easing.type: Easing.Linear
            from: play.height
            to: 0;
            onStarted: pause.enabled = play.enabled = false;
            onStopped: pause.enabled = play.enabled = true;
        }
    }

    ControlItemBlock{
        blockWidth: 2
        Label{
            elide: Text.ElideRight
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: controlData?controlData.name:"null"
        }
    }

    popup: Popup{
        id:popup
        GridLayout{
            columns: 2
            Label{
                text:"Name  : "
                Layout.bottomMargin: 5
            }
            TextInputField{
                enabled: UserManagment.currentUser.havePermission(Permission.CHANGE_GROUP_NAME);
                Layout.fillWidth: true
                text: controlData?controlData.name:"null"
                onTextChanged: if(controlData)controlData.name = text
            }

            Label{
                text:"Activate Cooldown"
            }
            TextInputField{
                Layout.fillWidth: true
                text: controlData?controlData.activateCooldown:100
                validator: IntValidator{
                    bottom:0
                    top:100000
                }
                onTextChanged: if(controlData)controlData.activateCooldown = text
                Text{
                    text:"ms"
                    color: Material.foreground
                    x : parent.contentWidth
                    anchors.verticalCenter: parent.verticalCenter
                }
                enabled: UserManagment.currentUser.havePermission(Permission.CHANGE_TIMEOUTS)
            }

            Label{
                text:"Deactivate Cooldown"
            }
            TextInputField{
                Layout.fillWidth: true
                text: controlData?controlData.deactivateCooldown:100
                validator: IntValidator{
                    bottom:0
                    top:100000
                }
                onTextChanged: if(controlData)controlData.deactivateCooldown = text
                Text{
                    text:"ms"
                    color: Material.foreground
                    x : parent.contentWidth
                    anchors.verticalCenter: parent.verticalCenter
                }
                enabled: UserManagment.currentUser.havePermission(Permission.CHANGE_TIMEOUTS)
            }


            Label{
                text:"Geräte : "
            }
            TextInputField{
                Layout.fillWidth: true
                id:search

                Text{
                    color: "grey"
                    anchors.baseline: parent.baseline
                    visible: parent.text.length===0
                    text: "search"
                }
            }

            ListView{
                enabled: UserManagment.currentUser.havePermission(Permission.CHANGE_GROUP_DEVICES);
                Layout.columnSpan: 2
                width: 200
                height: 125
                clip: true
                id: deviceList
                property var regex: new RegExp(search.text,"i");
                model: controlData?controlData.groupModel:null
                delegate: CheckDelegate{
                    width: 200
                    text: modelData.name
                    visible: height !== 0
                    height: deviceList.regex.test(modelData.name) ? implicitHeight : 0;
                    checked: use
                    onCheckedChanged: use = checked
                    clip: true
                    Behavior on height {
                        NumberAnimation{
                            duration: 250
                            easing.type: "OutQuad"
                        }
                    }
                }
            }
        }
        closePolicy: Popup.CloseOnPressOutside
    }
}
