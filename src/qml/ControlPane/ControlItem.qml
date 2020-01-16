import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.0
import custom.licht.template 1.0
import custom.licht 1.0

ControlItemTemplate{

    controlData: __controlData
    x: __controlData.startXBlock * 50
    y: __controlData.startYBlock * 50
    moveable: UserManagment.currentUser.havePermission(Permission.MOVE_CONTROL_ITEM);
    visible: controlData.isVisibleForUser
    id:item
    property bool pressed: false
    onPressedChanged:{
        if(pressed){
            rectangle.color=Qt.rgba(0,0,1,.2);
        }else{
            rectangle.color="transparent";
        }
    }

    Rectangle{
        id:rectangle
        anchors.fill: parent
        border.width: 1
        border.color: "blue"
        color: "transparent"
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

    Button{
        id: settings
        padding: 1
        width: 20
        height: 20
        anchors.top: parent.top
        anchors.right: parent.right
        icon.source: "/icons/ic_settings_black_24px.svg"
        background: null
        opacity: 0
        visible: popup!==null

        Behavior on rotation{
            NumberAnimation{
            }
        }
        Behavior on opacity{
            NumberAnimation{
            }
        }
        onClicked: {
            let pos = mapToItem(item.parent,pressX,pressY);
            // do not move the popup out of the viewport
            pos.x = Math.min(pos.x,item.parent.width-popup.width-5);
            pos.y = Math.min(pos.y,item.parent.height-popup.height-5);
            pos = item.mapFromItem(item.parent,pos.x,pos.y);
            popup.x = pos.x;
            popup.y = pos.y;
            popup.open();
        }
        onHoveredChanged: {
            if(hovered){
                 settings.rotation = 90;
            }else{
                 settings.rotation = 0;
            }
        }
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
    property Menu menu: menu
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
            enabled: UserManagment.currentUser.havePermission(Permission.CHANGE_CONTROL_ITEM_VISIBILITY);
            text: "Visibility"
            onClicked: {
                visibilityPopup.x = menu.x;
                visibilityPopup.y = menu.y;
                visibilityPopup.visible = true;
            }

        }
        MenuItem{
            enabled: UserManagment.currentUser.havePermission(Permission.REMOVE_CONTROL_ITEM);
            text: "Delete"
            onClicked: {
                item.parent=null;
                item.destroy();
            }

        }
    }
    property Popup popup;
    property Popup visibilityPopup: visibilityPopup;

    Popup{
        id: visibilityPopup
        width: 200
        implicitHeight: visibilityListView.contentHeight + 20 + label.height + 10
        padding: 0
        contentItem: ColumnLayout{
            spacing: 0
            Label{
                id: label
                Layout.margins: 10
                Layout.fillWidth: true
                wrapMode: "WordWrap"
                text: "Which user should see this control item?"
            }
            ListView{
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.bottomMargin: 10
                id: visibilityListView
                clip: true
                model: controlData.userVisibilityModel
                delegate: CheckDelegate{
                    width: visibilityListView.width
                    checked: isVisible
                    text: userName
                    onCheckedChanged: if(checked !== isVisible) isVisible = checked
                }
            }
        }
    }
}
