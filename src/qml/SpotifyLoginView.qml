import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.3

Item{
    ListView{
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.bottom: buttonNewUser.top
        id:listView
        model:spotify.knownUser
        clip: true
        delegate: ItemDelegate{
            width: listView.width
            Component.onCompleted: {
                background.color = Qt.binding(function(){return (spotify.currentUser? spotify.currentUser.id === modelData.id : false)?"lightblue":"white";});
            }
            text: (modelData.userName?modelData.userName:"")+" ("+modelData.email+")"
            Button{
                visible: spotify.currentUser ? modelData.id !== spotify.currentUser.id : true
                width: 80
                id:buttonLogin
                text: "Login"
                anchors.right: parent.right
                anchors.top:parent.top
                anchors.bottom: parent.bottom
                anchors.rightMargin: 5                
                onClicked: {
                    modelData.login();
                }
            }
            Label{
                visible: spotify.currentUser ? modelData.id === spotify.currentUser.id : false
                width: 80
                text: "Logged in"
                verticalAlignment: "AlignVCenter"
                horizontalAlignment: "AlignHCenter"
                color: "grey"
                anchors.right: parent.right
                anchors.top:parent.top
                anchors.bottom: parent.bottom
                anchors.rightMargin: 5
            }
        }
    }

    Button{
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        id: buttonNewUser
        text:"Login with Spotify Account"
        font.pixelSize: 15
        onClicked: spotify.loginNewUser();
    }

}
