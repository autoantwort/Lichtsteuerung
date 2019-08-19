import QtQuick 2.12
import QtQuick.Controls 2.12

RoundButton {
    text: "?"
    property string tooltipText: "Help"
    property double defaultOpacity: 0.7
    ToolTip.visible: hovered
    ToolTip.text: tooltipText
    ToolTip.delay: 500

    anchors.right: parent.right
    anchors.bottom: parent.bottom
    opacity: hovered ? 1 : defaultOpacity
    hoverEnabled: true
    Behavior on opacity{
        NumberAnimation{
            duration: 200
            easing.type: Easing.OutQuint
        }
    }
}
