import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import custom.licht 1.0
import "components"

Item{
    property alias currentItem : listView.currentItem
    ColumnLayout{
        anchors.fill: parent
        ListView{
            id:listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredHeight: model.rowCount() * 50
            clip: true
            highlightMoveDuration: 100
            highlightResizeDuration: 100
            model:userModel
            highlightFollowsCurrentItem: true
            delegate: SwipeDelegate{
                swipe.left: Label {
                    id: deleteLabel
                    text: qsTr("Delete")
                    color: "white"
                    verticalAlignment: Label.AlignVCenter
                    padding: 12
                    height: parent.height
                    //SwipeDelegate.onClicked:
                    background: Rectangle {
                        color: deleteMouseArea.pressed ? Qt.darker("tomato", 1.1) : "tomato"
                    }
                    MouseArea{
                        id: deleteMouseArea
                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton
                        onClicked: askToDeletePopup.deleteUser(modelData)
                    }
                }
                Timer{
                    id:closeTimer
                    interval: 2000
                    onTriggered: parent.swipe.close()
                }
                swipe.onOpened: {
                    closeTimer.running = true;
                }
                swipe.enabled: modelData !== UserManagment.defaultUser && UserManagment.currentUser.havePermission(Permission.Admin);
                anchors.margins: 5
                clip:true
                width: parent.width
                Component.onCompleted: {
                    background.color = Qt.binding(function(){return UserManagment.currentUser === modelData?"lightblue":"transparent";});
                }
                Behavior on height {
                    NumberAnimation{
                        duration: 500
                        easing.type: Easing.OutQuart
                    }
                }
                TextInputField{
                    anchors.verticalCenter: parent.contentItem.verticalCenter
                    x: parent.contentItem.x + 10
                    text: modelData.name
                    width: Math.max(implicitWidth+20,70)
                    enabled: (UserManagment.currentUser.havePermission(Permission.Admin) || modelData === UserManagment.currentUser)&&modelData!==UserManagment.defaultUser
                }
                Button{
                    visible: UserManagment.currentUser.havePermission(Permission.Admin);
                    text:"Change Permissions and Auto-Login"
                    anchors.right: buttonChangePassword.left
                    anchors.top: buttonChangePassword.top
                    anchors.bottom: buttonChangePassword.bottom
                    anchors.rightMargin: 5
                    onClicked: {
                        userSettingsPopup.show(modelData);
                    }
                }
                Button{
                    id:buttonChangePassword
                    visible: modelData === UserManagment.currentUser && modelData !== UserManagment.defaultUser;
                    text: "Change Password"
                    anchors.right: buttonLogin.left
                    anchors.top: buttonLogin.top
                    anchors.bottom: buttonLogin.bottom
                    anchors.rightMargin: 5
                }
                Button{
                    width: 80
                    id:buttonLogin
                    enabled: !(modelData===UserManagment.defaultUser&&modelData===UserManagment.currentUser)
                    text: modelData===UserManagment.currentUser?"Logout":"Login"
                    anchors.right: parent.right
                    anchors.top:parent.top
                    anchors.bottom: parent.bottom
                    anchors.rightMargin: 5
                    onClicked: {
                        if(modelData===UserManagment.defaultUser){
                            UserManagment.logout();
                        }else if(modelData===UserManagment.currentUser){
                            UserManagment.logout()
                        }else{
                            dialog.user = modelData;
                            dialog.visible = true;
                        }
                    }
                }
            }
        }

        Button{
            Layout.fillWidth: true
            Layout.leftMargin: 5
            Layout.rightMargin: 5
            id: buttonAdd
            text:"+"
            font.pixelSize: 15
            onClicked: addUserDialog.visible=true;
            enabled: UserManagment.currentUser.havePermission(Permission.Admin);
        }
        Label{
            Layout.topMargin: 10
            Layout.leftMargin: 5
            text: "Known Spotify Accounts"
            font.pixelSize: 18
        }
        Rectangle{
            color: "lightgrey"
            Layout.fillWidth: true
            height: 1
        }


        SpotifyLoginView{
            id: spotifyLoginView
            Layout.fillHeight: true
            Layout.fillWidth: true
            implicitHeight: 200;
        }
    }


    function login(){

        if(UserManagment.login(dialog.user,password.text)){
            dialog.visible = false;
        }else{
            password.underlineColor = "red";
            cosAni.from = dialog.x+30;
            cosAni.to = dialog.x-30;
            fromToAni.to = dialog.x;
            cosAni.start();
            fromToAni.start();
        }

    }
    NumberAnimation {
        id:cosAni
        target: dialog
        property: "x"
        duration: 150
        easing.type: Easing.CosineCurve
        onStopped: if(from!==to)running=true
    }
    NumberAnimation {
        id:fromToAni
        target: cosAni
        properties: "to,from"
        duration: 500
        easing.type: Easing.Linear
    }

    Dialog{
        modality: Qt.WindowModal
        id:dialog
        title: "Enter Password"
        width:300
        property var user;
        onVisibleChanged: {
            if(visible)
                password.forceActiveFocus()
            password.text="";
        }
        contentItem:Pane{
            ColumnLayout{
                anchors.fill: parent
                RowLayout{
                    Label{
                        text:"Passwort:"
                    }
                    TextInputField{
                        echoMode: TextInput.Password
                        Layout.fillWidth: true
                        id: password
                        onAccepted: login();
                        KeyNavigation.tab:  loginButton
                    }
                }
                RowLayout{
                    Button{
                        Layout.fillWidth: true
                        text:"Abbrechen"
                        onClicked: dialog.visible = false
                    }
                    Button{
                        id:loginButton
                        Layout.fillWidth: true
                        text:"Einloggen"
                        onClicked: login()
                    }
                }
            }
        }
    }
    Dialog{
        modality: Qt.WindowModal
        id:addUserDialog
        title: "Create new user."
        width:300
        onVisibleChanged:{
            if(visible){
                username.text = "";;
                password1.text = "";
                password2.text = "";
            }
        }
        contentItem:
            Pane{
            GridLayout{
                anchors.fill: parent
                columns: 2
                Label{
                    text:"Username : "
                }
                TextInputField{
                    Layout.fillWidth: true
                    id:username
                    KeyNavigation.tab: password1
                    onAccepted: password1.forceActiveFocus()
                }
                Label{
                    text:"Password : "
                }
                TextInputField{
                    Layout.fillWidth: true
                    id:password1
                    echoMode: TextInput.Password
                    KeyNavigation.tab: password2
                    onAccepted: password2.forceActiveFocus()
                }
                Label{
                    text:"Password : "
                }
                TextInputField{
                    Layout.fillWidth: true
                    id:password2
                    echoMode: TextInput.Password
                    KeyNavigation.tab: createUser
                    onAccepted: createUser.forceActiveFocus()
                }

                Button{
                    id:dontCreateUser
                    text:"Abbrechen"
                    KeyNavigation.tab: createUser
                    onClicked: addUserDialog.visible = false
                }
                Button{
                    id:createUser
                    KeyNavigation.tab: dontCreateUser
                    Layout.fillWidth: true
                    text:"Create"
                    onClicked:{
                        if(username.text.length<3){
                            username.underlineColor = "red";
                            return;
                        }else{
                            username.underlineColor = "lightgreen";
                        }
                        if(password1.length<5){
                            password1.underlineColor = "red";
                            return;
                        }else{
                            password1.underlineColor="lightgreen";
                        }
                        if(password1.text!==password2.text){
                            password2.underlineColor="red";
                            return;
                        }else{
                            password2.underlineColor="lightgreen";
                        }
                        UserManagment.addUser(username.text,password1.text);
                        addUserDialog.visible=false;
                    }
                }
            }
        }
    }

    AskWhenRemovePopup{
        id: askToDeletePopup
        property var _user
        text: "Do you really want to delete the user " + (_user ? '"' + _user.name + '"' : "error")
        function deleteUser(user){
            _user = user;
            open();
        }
        onYesClicked: UserManagment.removeUser(_user);
    }

    UserSettingsPopup{
        id: userSettingsPopup
    }

}
