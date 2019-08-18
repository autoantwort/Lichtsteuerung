import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import ".."

Window{
    property VerticalTabButton button: null
    property LedVisualisationView view: null
    flags: Qt.WindowStaysOnTopHint | Qt.Dialog | Qt.WindowCloseButtonHint | Qt.WindowTitleHint
    title: "Virtual LED Stripes"
    // TODO: reuse component from SwipeView (does not work on first try)
    LedVisualisationView{
        id: root
        anchors.fill: parent
        inOwnWindow: true
    }
}
