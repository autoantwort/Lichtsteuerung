import QtQuick 2.0

QtObject {
    property string titel
    property string explanation
    property Item component
    property bool visible: true
    enum Position{
        East, South, North, West
    }
    property int position: HelpEntry.Position.West
    signal leave;
    signal enter;
}
