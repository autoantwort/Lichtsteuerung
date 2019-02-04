import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtGraphicalEffects 1.0
import custom.licht.template 1.0
import custom.licht 1.0

ControlItemTemplate{

    moveable: UserManagment.currentUser.havePermission(Permission.MOVE_CONTROL_ITEM);
    id:item
    property bool pressed: false
    onPressedChanged:{
        if(pressed){
            rectangle.color="#C7C7FF";
        }else{
            rectangle.color="white";
        }
    }

    Rectangle{
        id:rectangle
        anchors.fill: parent
        border.width: 1
        border.color: "blue"
        radius: 3
        layer.enabled: true;
        layer.effect: FastBlur{
            id: meinBlur
            radius: 8
        }
        Behavior on color{

            ColorAnimation {
                duration: 200
            }
        }
    }
    onSettingVisibleChange: settings.opacity = visible

    Image {
        opacity: 0
        id: settings
        width: 18
        height: 18
        anchors.top: parent.top
        anchors.right: parent.right
        source: "icons/ic_settings_black_24px.svg"
        visible: popup!==null

        Behavior on rotation{
            NumberAnimation{
            }
        }
        Behavior on opacity{
            NumberAnimation{
            }
        }

        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            onEntered: settings.rotation=90
            onExited: settings.rotation=0
            onClicked: {
                var pos = mapToItem(parent.parent,mouse.x,mouse.y);
                popup.x = pos.x;
                popup.y = pos.y;
                popup.open();
            }
        }
    }
    Item {
        x:0;
        y:0;
    }

    Behavior on x{
        NumberAnimation{
            easing.type: Easing.OutExpo
        }
    }

    Behavior on y{
        NumberAnimation{
            easing.type: Easing.OutExpo
        }
    }
    onOpenPopup: {
        menu.x = x;
        menu.y = y;
        menu.open();
    }
    Menu{
        id: menu
        MenuItem{
            visible: popup!==null
            height: visible*implicitHeight
            text: "Settings"
            onClicked: {
                popup.x = menu.x;
                popup.y = menu.y;
                popup.open();
            }
        }
        MenuItem{
            enabled: UserManagment.currentUser.havePermission(Permission.REMOVE_CONTROL_ITEM);
            text: "Delete"
            onClicked: {
                item.parent=null;
                delete item;
            }

        }
    }
    property Popup popup;
}
