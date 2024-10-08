import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import Qt5Compat.GraphicalEffects as GE
import QtQuick.Shapes 1.3
import custom.licht 1.0
import "components"

ColumnLayout{
    id: root
    anchors.margins: 5
    property var toPlacedDevice: null

    ToolTip.visible: !root.toPlacedDevice && hoverMouseArea.containsMouse
    ToolTip.text: "Use the shift or alt modifier to move the control point or no modifier to move the map"

    SwipeView.onIsCurrentItemChanged: {
        if(!SwipeView.isCurrentItem){
            toPlacedDevice = null; // if the item is not placed and we leave the tab ignore the to placed item
        }
    }

    Item{
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.preferredHeight: 9999
        Rectangle{
            id: borderRectangle
            width: deviceInfo.visible ? parent.width - deviceInfo.width - deviceInfo.anchors.leftMargin - deviceInfo.anchors.rightMargin : parent.width
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            border.width: 2
            border.color: "black"
            color: "transparent"

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
                        ToolTip.visible: root.toPlacedDevice
                        ToolTip.text: "Click into the map to place " + (root.toPlacedDevice?root.toPlacedDevice.name:"nothing")
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
                        MouseArea{
                            anchors.fill: parent
                            acceptedButtons: Qt.LeftButton | Qt.RightButton
                            enabled: root.toPlacedDevice !== null
                            onClicked: {
                                if(root.toPlacedDevice && mouse.button === Qt.LeftButton){
                                    root.toPlacedDevice.position.x = mouse.x - map.translation.x;
                                    root.toPlacedDevice.position.y = mouse.y - map.translation.y;
                                }else{
                                    mouse.accepted = false;
                                }
                                root.toPlacedDevice = null;
                                deviceInfo.device = null;
                            }
                        }
                        MouseArea{
                            id: hoverMouseArea
                            anchors.fill: parent
                            acceptedButtons: Qt.NoButton
                            hoverEnabled: true
                            scrollGestureEnabled: true
                            onWheel: {
                                if(wheel.modifiers & Qt.ControlModifier){
                                    var newScale = map.scale + wheel.angleDelta.y / 120. * 0.1;
                                    map.scale = Math.max(0.2,Math.min(newScale,3));
                                    wheel.accepted = true;
                                }else{
                                    wheel.accepted = false
                                }
                            }
                        }

                        Repeater{
                            model: deviceModel
                            Rectangle{
                                visible: modelData.position.x !== -1 && modelData.position.y !== -1
                                property int selectScale: 1+(deviceInfo.device === modelData)
                                width: 5 * selectScale
                                height: 5 * selectScale
                                radius: 5 * selectScale
                                color: "yellow"
                                x: modelData.position.x - 2 * selectScale + map.translation.x
                                y: modelData.position.y - 2 * selectScale + map.translation.y
                                GE.RadialGradient{
                                    anchors.fill: parent
                                    anchors.margins: -10
                                    gradient: Gradient {
                                        GradientStop { position: 0.2; color: Qt.rgba(1.,1.,1.,modelData.prototype.channel.rowCount() > 0 ? dmxOutputValues.values ? dmxOutputValues.values.value(modelData.startDMXChannel).value/255. : 0 : 0) }
                                        GradientStop { position: 0.5; color: Qt.rgba(1,1,1,0) }
                                    }
                                    z: parent.z - 1
                                }
                                MouseArea{
                                    id: mouseArea
                                    enabled: !root.toPlacedDevice
                                    anchors.fill: parent
                                    anchors.margins: -5
                                    hoverEnabled: true
                                    onClicked: deviceInfo.device = modelData
                                    drag.target: parent
                                    drag.minimumX: 0
                                    drag.minimumY: 0
                                    drag.maximumX: map.width - 5
                                    drag.maximumY: map.height - 5
                                    drag.threshold: UserManagment.currentUser.havePermission(Permission.CHANGE_POSITION) ? 0 : 999999; // "disable dragging"
                                    onReleased: {
                                        // If we dont use variables the y coordinate gets resetted after setting modelData.position.x
                                        var newX = parent.x + 2 * parent.selectScale - map.translation.x;
                                        var newY = parent.y + 2 * parent.selectScale - map.translation.y;
                                        modelData.position.x = newX;
                                        modelData.position.y = newY;
                                    }
                                }

                                ToolTip.visible: mouseArea.containsMouse
                                ToolTip.delay: 500
                                ToolTip.text: modelData.name + "\nDmx Addess: " + modelData.startDMXChannel
                            }
                        }

                        Rectangle{
                            id: controlPoint
                            x: parent.controlPoint.x + map.translation.x - 3
                            y: parent.controlPoint.y + map.translation.y - 3
                            width: 7
                            height: 7
                            radius: 7
                            color: "lightgreen"
                            MouseArea{
                                id: mouseAreaControlPoint
                                acceptedButtons: Qt.NoButton
                                enabled: !root.toPlacedDevice
                                anchors.fill: parent
                                anchors.margins: -5
                                hoverEnabled: true
                            }
                            ToolTip.visible: mouseAreaControlPoint.containsMouse
                            ToolTip.delay: 500
                            ToolTip.text: "This control point is used by modules programs"
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
        ColumnLayout{
            anchors.margins: 10
            anchors.left: borderRectangle.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            visible: width > 0
            id: deviceInfo
            property var device: null
            width: device ? 200 : 0
            Behavior on width {
                NumberAnimation{
                    duration: 100
                    easing.type: "OutCubic"
                }
            }
            Layout.fillHeight: true
            Label{
                text: deviceInfo.device ? deviceInfo.device.name + " (" + deviceInfo.device.prototype.name + ")" : "No Selected Device"
                font.pixelSize: 20
                font.bold: true
                Layout.bottomMargin: 10
            }
            RowLayout{
                Label{
                    text:"x:"
                }
                TextInputField{
                    enabled: UserManagment.currentUser.havePermission(Permission.CHANGE_POSITION);
                    Layout.minimumWidth: 50
                    text: deviceInfo.device ? deviceInfo.device.position.x : ""
                    validator: IntValidator{}
                    onTextChanged: if(deviceInfo.device) deviceInfo.device.position.x = text.length?text:-1
                }
                Label{
                    Layout.leftMargin: 10
                    text:"y:"
                }
                TextInputField{
                    enabled: UserManagment.currentUser.havePermission(Permission.CHANGE_POSITION);
                    text: deviceInfo.device ? deviceInfo.device.position.y : ""
                    validator: IntValidator{}
                    onTextChanged:if(deviceInfo.device) deviceInfo.device.position.y = text.length?text:-1
                }
                Layout.bottomMargin: 10
            }
            Repeater{
                model: deviceInfo.device ? deviceInfo.device.prototype.channel : null
                Item{
                    Layout.fillWidth: true
                    Layout.bottomMargin: -10
                    implicitHeight: label.implicitHeight + slider.implicitHeight + slider.anchors.topMargin
                    Label{
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.right: parent.right
                        id: label
                        text: name
                        font.underline: true
                    }
                    Label{
                        width: 30
                        id: valueLabel
                        text: slider.value
                        anchors.left: parent.left
                        anchors.verticalCenter: slider.verticalCenter
                    }

                    Slider{
                        id: slider
                        anchors.top: label.bottom
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        anchors.left: valueLabel.right
                        anchors.topMargin: -8

                        value: deviceInfo.device ? deviceInfo.device.getFilterForChannelindex(index).value : 0
                        from: 0
                        to: 255
                        stepSize: 1
                        onValueChanged: deviceInfo.device.getFilterForChannelindex(index).value = value
                    }
                }
            }
            Item{
                Layout.fillHeight: true
            }
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
                color: Material.foreground
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
