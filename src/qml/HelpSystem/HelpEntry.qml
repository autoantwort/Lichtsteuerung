import QtQuick 2.0

QtObject {
    property string titel
    property string explanation
    property Item component
    property bool visible: true
    property real xShift: 0
    property real yShift: 0
    enum Position{
        East, South, North, West
    }
    property int position: parent.defaultPosition ? parent.defaultPosition : HelpEntry.Position.East
    signal leave;
    signal enter;
}
