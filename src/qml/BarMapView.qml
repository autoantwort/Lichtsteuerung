import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5
import custom.licht 1.0

ColumnLayout{
    anchors.margins: 5
    Rectangle{
        border.width: 2
        border.color: "black"
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.preferredHeight: 9999
        Flickable{
            anchors.fill: parent
            anchors.margins: parent.border.width
            id: flickable
            clip: true
            contentWidth: map.boundingBox.width
            contentHeight: map.boundingBox.height
            rightMargin: 30
            topMargin: 30
            bottomMargin: 30
            leftMargin: 30
            ScrollBar.horizontal: ScrollBar{}
            ScrollBar.vertical: ScrollBar{}

            Item{
                x : -map.boundingBox.x
                y : -map.boundingBox.y
                Behavior on x {
                    NumberAnimation{}
                    enabled: map.finishedRotationAnimation
                }
                Behavior on y {
                    NumberAnimation{}
                    enabled: map.finishedRotationAnimation
                }
                MapView{
                    property rect boundingBox: mapToItem(parent,0,0,width,height)
                    property rect lastBoundingBox
                    property real reachRotation : 0
                    property bool finishedRotationAnimation : false
                    rotation: reachRotation
                    onRotationChanged: {
                        if(rotation === reachRotation){
                            finishedRotationAnimation = true;
                            boundingBox = mapToItem(parent,0,0,width,height);
                            finishedRotationAnimation = false;
                        }
                    }
                    onBoundingBoxChanged: {
                        if(lastBoundingBox&&!finishedRotationAnimation){
                            flickable.contentX -= boundingBox.x - lastBoundingBox.x;
                            flickable.contentY -= boundingBox.y - lastBoundingBox.y;
                        }
                        lastBoundingBox = boundingBox
                    }
                    id: map
                    width: implicitWidth
                    height: implicitHeight
                    onScaleChanged: boundingBox = mapToItem(parent,0,0,width,height);
                    Behavior on rotation {
                        NumberAnimation{
                            duration: 400
                            easing.type: "InOutCubic"
                        }
                    }
                }
            }
        }
        PinchHandler{
            target: map
            minimumScale: 0.2
            maximumScale: 2
            maximumRotation: map.rotation
            minimumRotation: map.rotation
            xAxis.enabled: false;
            yAxis.enabled: false;
        }
    }


    RowLayout{
        Layout.fillWidth: true
        Label{
            text: "Scaling: "
        }
        Slider {
            from: 0.2
            to: 3
            value: map.scale
            onValueChanged: map.scale = value
            Layout.preferredWidth: 200
            Text {
                text: parent.value.toFixed(2)
                anchors.bottom: parent.handle.top
                anchors.bottomMargin: 2
                anchors.horizontalCenter: parent.handle.horizontalCenter
            }
        }
        Rectangle{
            width: 1
            color: "lightgrey"
            Layout.fillHeight: true
            Layout.leftMargin: 20
            Layout.rightMargin: 20
            Layout.topMargin: 5
            Layout.bottomMargin: 5
        }

        Label{
            text: "Rotation: "
        }
        Button{
            text: "Rotate CW"
            onClicked: map.reachRotation += 90;
            Layout.rightMargin: 4
        }
        Button{
            text: "Rotate CCW"
            onClicked: map.reachRotation -= 90;
        }
    }
}
