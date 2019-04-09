import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5
import custom.licht 1.0

Item {
    Label{
        id: label
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 5
        text: "FFT analysis, Frequencies in Hz:"
        font.underline: true
    }

    Flickable {
        id: flickable
        anchors.top: label.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: contentHeight + ScrollBar.horizontal.height + 5

        contentWidth: graph.implicitWidth * scaleID.xScale
        contentHeight: graph.height
        boundsBehavior: Flickable.DragOverBounds
        flickableDirection: Flickable.HorizontalFlick
        ScrollBar.horizontal: ScrollBar {
            policy: ScrollBar.AlwaysOn
        }

        Graph {
            id: graph
            height: 200
            width: implicitWidth
            transform: Scale {
                id: scaleID
                xScale: 1
                origin.y: 200
            }
        }

        Repeater {
            model: ((flickable.contentWidth + 40) / 40).toFixed(0)
            delegate: Text {
                text: ((index * 40 + 20) / graph.implicitWidth * (44100 / 2) * (1 / scaleID.xScale)).toFixed(0)
                x: index * 40 + 20 - contentWidth / 2
                textFormat: Text.PlainText
                y: 2
                Rectangle {
                    x: parent.contentWidth / 2
                    y: parent.y + parent.contentHeight
                    width: 1
                    height: 200 - y
                    color: "grey"
                }
            }
        }
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.NoButton
            onWheel: {
                flickable.flick(wheel.angleDelta.y / 120 * 1000, 0)
            }
        }
    }
    Item{
        anchors.top: flickable.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.leftMargin: 10
        anchors.topMargin: 5
        GridLayout{
            columns: 2

            Label{
                text: "X-Scale:"
            }
            Slider {
                from: 0.3
                to: 10
                value: 1
                onValueChanged: scaleID.xScale = value
                Layout.preferredWidth: 200
                Text {
                    text: parent.value.toFixed(2)
                    anchors.bottom: parent.handle.top
                    anchors.bottomMargin: 4
                    anchors.horizontalCenter: parent.handle.horizontalCenter
                }
            }

            Label{
                text: "Y-Scale:"
            }
            Slider {
                from: 0.3
                to: 5
                value: 1
                onValueChanged: scaleID.yScale = value
                Layout.preferredWidth: 200
                Text {
                    text: parent.value.toFixed(2)
                    anchors.bottom: parent.handle.top
                    anchors.bottomMargin: 4
                    anchors.horizontalCenter: parent.handle.horizontalCenter
                }
            }
        }
    }
}
