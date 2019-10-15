import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.0
import "../components"

Popup {
    property var user: null;
    id: popup
    bottomPadding: 0
    x: 15
    y: 15
    width: parent.width - 30
    height: parent.height - 30
    function show(newUser){
        user = newUser;
        visible = true;
    }
    Page{
        anchors.fill: parent
        header: TabBar{
            id: tabBar
            currentIndex: swipeView.currentIndex
            TabButton{
                text: "Permissions"
            }
            TabButton{
                text: "Auto Login"
            }
        }
        contentItem: SwipeView{
            clip: true
            id: swipeView
            currentIndex: tabBar.currentIndex
            interactive: false
            ListView{
                clip: true
                id: permissionsView
                model: user?user.permissionModel:null
                delegate: SwitchDelegate{
                    width: permissionsView.width
                    checked: havePermission
                    onCheckedChanged: if(havePermission !== checked) havePermission = checked
                    text: permissionName
                }
            }
            Item{
                ColumnLayout{
                    anchors.fill: parent
                    anchors.margins: 5
                    Label{
                        Layout.fillWidth: true
                        text: "A user can be auto logged in in the light control, when a specific OS user is currently logged in. In this case no password is required.\nThe user name of the currently logged in os user is: " + UserManagment.currentOsUserName + "\nHere you can specify the usernames for the account " + (user?user.name:"null") + ":"
                        wrapMode: "WordWrap"
                        Layout.margins: 5
                    }
                    ListView{
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true
                        id: usernameView
                        model: user?user.autologinUsernames:null
                        delegate: ItemDelegate{
                            width: usernameView.width
                            text: modelData
                            onClicked: usernameView.currentIndex = index
                        }
                        highlight: Rectangle{
                            color: "blue"
                            opacity: 0.7
                        }
                        Rectangle{
                            anchors.fill: parent
                            border.width: 1
                            border.color: "gray"
                            color: "#00000000"
                            radius: 3
                            layer.enabled: true;
                            layer.effect: FastBlur{
                                radius: 8
                            }
                        }
                    }
                    RowLayout{
                        Layout.fillWidth: true
                        Label{
                            text: "Username: "
                        }
                        TextInputField{
                            Layout.fillWidth: true
                            id: username
                            text: usernameView.currentItem?usernameView.currentItem.text:""
                        }
                        Button{
                            text: "Change"
                            enabled: usernameView.currentItem && username.text.length > 0
                            onClicked: user.changeAutologinUsername(usernameView.currentIndex,username.text)
                        }
                        Button{
                            text: "Add"
                            enabled: username.text.length > 0
                            onClicked: {
                                user.addAutologinUsername(username.text);
                                usernameView.currentIndex = -1;
                            }
                        }
                        Button{
                            text: "Remove"
                            enabled: usernameView.currentItem
                            onClicked: user.removeAutologinUsername(usernameView.currentIndex);
                        }
                    }
                }
            }
        }
        footer: RowLayout {
            layoutDirection: Qt.RightToLeft
            Button{
                Layout.fillWidth: true
                Layout.margins: 4
                text: "Close"
                onClicked: popup.visible = false;
            }
        }
    }
}
