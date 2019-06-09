import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import "../../"

Label {
    id: label
    property double speed: 100
    NumberAnimation{
        target: label
        property: "leftPadding"
        duration: (label.width + label.contentWidth)/100 * (1/speed) * 1000
        from: label.width + label.contentWidth
        onFromChanged: restart()
        to: -label.contentWidth
        running: true
        loops: Animation.Infinite
    }

}
