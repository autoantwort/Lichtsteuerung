import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import custom.licht 1.0

Label{
    z: 10
    clip: true
    background: Rectangle{
        color:"lightblue"
    }
    MouseArea{
        hoverEnabled: true
        anchors.fill: parent
        id:mouseArea
        acceptedButtons: Qt.NoButton
    }

    ToolTip.visible: mouseArea.containsMouse
    ToolTip.delay: 1000
    ToolTip.text: text
    id:label
}
