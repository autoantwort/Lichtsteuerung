import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.3

CenteredPopup{
    property alias label: label
    property alias text: label.text
    property alias textFont: label.font
    signal yesClicked();
    id: popup
    ColumnLayout{
        Label{
            id: label
            Layout.margins: 5
            text: "Do you really want to delete the selected Entry?"
            font.pixelSize: 16
        }
        RowLayout{
            Layout.topMargin: -5
            Layout.bottomMargin: -5
            Item{
                Layout.fillWidth: true
            }
            Button{
                flat: true
                text: "Cancel"
                Component.onCompleted: contentItem.color = Material.accent
                onClicked: popup.visible = false;
            }
            Button{
                flat: true
                id: deleteButton
                text: "Yes"
                Component.onCompleted: contentItem.color = Material.accent
                Keys.onEnterPressed: {yesClicked();popup.close()}
                Keys.onReturnPressed: {yesClicked();popup.close()}
                onClicked: {yesClicked();popup.close()}
            }
        }
    }
    onOpened: {forceActiveFocus();deleteButton.focus = true}
}
