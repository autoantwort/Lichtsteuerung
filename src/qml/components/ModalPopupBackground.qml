import QtQuick 2.12
import QtGraphicalEffects 1.0
import QtQuick.Window 2.12
import custom.licht 1.0

Rectangle{
    color: Qt.rgba(0,0,0,[.2, .506, .75][Settings.popupBackgroundEffectIntensity])
    FastBlur {
        source: Window.window ? Window.window.contentItem : null
        radius: [16,32,64][Settings.popupBackgroundEffectIntensity]
        anchors.fill: parent
        visible: Settings.popupBackgroundEffect === PopupBackground.Blur
    }
    Behavior on opacity { NumberAnimation { duration: 150 } }
}
