import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Shapes 1.0
import custom.licht 1.0

Shape{
    property alias color : pathObj.fillColor
    anchors.fill: parent;
    property alias targetStartIndex: pathObj.startX
    property alias targetLength: topRight.relativeX
    property alias sourceStartIndex: bottomLeft.x
    property real sourceLength
    property real targetBaseline
    property real sourceBaseline

    ShapePath{
        id:pathObj
        /* Path :
           * -> *
           |     \
           * <--- *
        */
        startY: targetBaseline
        strokeWidth: 0
        strokeColor: "transparent"
        PathLine{
            id: topRight;
            y: targetBaseline
        }
        PathCubic{
            y:sourceBaseline
            x:bottomLeft.x+sourceLength
            relativeControl1Y: (sourceBaseline-targetBaseline)/2
            relativeControl2Y: relativeControl1Y
            relativeControl1X: 0
            control2X: x
        }
        PathLine{
            id:bottomLeft
            y: sourceBaseline
        }
        PathCubic{

            x:pathObj.startX
            y:pathObj.startY
            relativeControl1Y: -(sourceBaseline-targetBaseline)/2
            relativeControl2Y: relativeControl1Y
            relativeControl1X: 0
            control2X: x
        }

    }
    /*MouseArea{
        x: (targetStartIndex + sourceStartIndex)/2
        y: targetBaseline
        width: (targetLength + sourceLength)/2
        height: (sourceBaseline-targetBaseline)
        onClicked: color = "red"
    }*/
}
