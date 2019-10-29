import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12


TextInput{
    color: Material.foreground
    selectByMouse: true
    font.pixelSize: 15
    selectionColor: Material.accentColor
    selectedTextColor: Material.primaryHighlightedTextColor
    bottomPadding: 3
    topPadding: 3
    clip: contentWidth > width || underline.width > width
    property alias underlineColor : underline.color
    property alias underline: underline
    cursorDelegate: Rectangle{
        color: Material.accentColor
        x: parent.cursorRectangle.x
        y: parent.cursorRectangle.y
        width: parent.cursorRectangle.width+1
        height: parent.cursorRectangle.height
        visible: parent.cursorVisible
        Behavior on x {
            NumberAnimation { easing.type: Easing.OutCubic; easing.amplitude: 9; easing.period: 50.0; duration: 500 }
        }
    }
    TextUnderline{
        id: underline
    }
}
