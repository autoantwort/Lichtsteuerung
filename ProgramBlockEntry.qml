import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import custom.licht 1.0
import Qt.labs.handlers 1.0

Label{
    z: 10
    //DragHandler{}
    clip: true
    background: Rectangle{
        color:"lightblue"
    }
    id:label
    ToolTip.text: label.text
    ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
    //ToolTip.enabled: label.width<label.contentWidth
}
