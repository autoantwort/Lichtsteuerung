import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Shapes 1.13
import QtQuick.Controls.Material 2.12
import custom.licht 1.0
import QtQml 2.12
import ".."
import "../components"

ControlItem{
    id:item
    blockWidth: 1
    blockHeight: 4



    ControlItemBlock{
        blockWidth: 1
        id:withRef
        clip:true
        Label{
            id: textLabel
            width: Math.max(implicitWidth,withRef.width)
            leftPadding: 4
            rightPadding: 4
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: controlData?controlData.name:"null"
            //elide: "ElideRight"
            font.pointSize: 12
            Behavior on x{
                NumberAnimation {
                    alwaysRunToEnd: true
                    duration: (textLabel.implicitWidth- withRef.width)*15
                }
            }
            Timer{
                id:timer
            }
            onXChanged: {
                if( x === withRef.width-textLabel.implicitWidth){ //end of animation
                    if(!mouseArea.containsMouse){
                        timer.interval = 500;
                        timer.triggered.connect(function(){
                            textLabel.x = 0;
                        });
                        timer.start();
                    }
                }
            }

            MouseArea{
                id:mouseArea
                anchors.fill: parent
                anchors.topMargin: 18
                anchors.bottomMargin: 18
                hoverEnabled: textLabel.implicitWidth > withRef.width
                acceptedButtons: Qt.NoButton
                preventStealing: true
                propagateComposedEvents: true
                onEntered: {
                    if(textLabel.implicitWidth > withRef.width){
                        textLabel.x = withRef.width-textLabel.implicitWidth;
                    }
                }
                onExited: {
                    if(textLabel.x === withRef.width-textLabel.implicitWidth){
                        textLabel.x = 0;
                    }
                }
                /*Rectangle{
                    anchors.fill: parent
                    color: "blue"
                }*/
            }
        }
    }
    ControlItemBlock{
        blockHeight: 3
        Slider{
            anchors.fill: parent
            anchors.bottomMargin: 20
            anchors.topMargin: -15
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            orientation: "Vertical"
            from: 0
            to: 255
            stepSize: 1
            Text {
                text: parent.value.toFixed(0)
                anchors.top:parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter

            }
            onValueChanged: controlData.value = value
            value: controlData.value;
        }
    }

    popup: Popup{
        id:popup
        ColumnLayout{
            RowLayout{
                Label{
                    text:"Name : "
                    horizontalAlignment: TextInput.AlignRight
                }
                TextInput{
                    enabled: UserManagment.currentUser.havePermission(Permission.CHANGE_GROUP_NAME);
                    Layout.fillWidth: true
                    text:controlData?controlData.name:"null";
                    onTextChanged: if(controlData)controlData.name=text;
                }
            }
            Rectangle{
                Layout.fillWidth: true
                height: 1
                color: "black"
            }
            RowLayout{
                enabled: UserManagment.currentUser.havePermission(Permission.CHANGE_MIN_MAX_MAPPING);
                ColumnLayout{
                    spacing: 2
                    Text {
                        text: qsTr("Min")
                        font.underline: true
                        Layout.alignment: Qt.AlignHCenter
                    }
                    RadioButton{
                        topPadding: 2
                        bottomPadding: 2
                        text: "Remap"
                        checked: controlData?controlData.minOperation==1/*REMAP*/:false
                        onCheckedChanged: controlData.minOperation = 1
                    }
                    RadioButton{
                        topPadding: 2
                        bottomPadding: 2
                        text: "Cut"
                        checked: controlData?controlData.minOperation==0/*CUT*/:false
                        onCheckedChanged: controlData.minOperation = 0
                    }

                }
                ColumnLayout{
                    spacing: 2
                    Text {
                        text: qsTr("Max")
                        font.underline: true
                        Layout.alignment: Qt.AlignHCenter
                    }
                    RadioButton{
                        topPadding: 2
                        bottomPadding: 2
                        text: "Remap"
                        checked: controlData?controlData.maxOperation==1/*REMAP*/:false
                        onCheckedChanged: controlData.maxOperation = 1
                    }
                    RadioButton{
                        topPadding: 2
                        bottomPadding: 2
                        text: "Cut"
                        checked: controlData?controlData.maxOperation==0/*CUT*/:false
                        onCheckedChanged: controlData.maxOperation = 0
                    }
                }
            }
            Shape {
                Layout.fillWidth: true
                Layout.leftMargin: 12
                Layout.rightMargin: 12
                Layout.topMargin: 10
                height: 30
                id: shape
                ShapePath {
                    strokeWidth: 1
                    strokeColor: "black"
                    joinStyle: ShapePath.MiterJoin
                    fillGradient: LinearGradient {
                        function getColor(value){
                            return Qt.rgba(value, value, 0, 1);
                        }
                        id: data
                        x1: 0; y1:shape.height/2;
                        x2: shape.width; y2: shape.height/2;
                        GradientStop { position: 0; color: data.getColor(0) }
                        GradientStop { position: 1; color: data.getColor(1) }
                    }
                    startX: 0; startY: 0
                    PathLine { x: shape.width; y: 0 }
                    PathLine { x: shape.width; y: shape.height }
                    PathLine { x: 0; y: shape.height }
                    PathLine { x: 0; y: 0 }
                }
                Rectangle{
                    color: Material.accentColor;
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    width: 1
                    id: positionRectangle
                    Text{
                        textFormat: Text.PlainText
                        font.family: "Courier New"
                        font.bold: true
                        text: parent.position
                        anchors.bottom: parent.top
                        anchors.horizontalCenter: parent.horizontalCenter
                    }

                    property int position: 0;
                    onPositionChanged: {
                        var minCut = controlData.minOperation==0;
                        var maxCut = controlData.maxOperation==0;
                        //code from DXMChannelFilter:
                        if(!minCut&&!maxCut){
                            x = (position * (rangeSlider.second.value-rangeSlider.first.value)/255.) + rangeSlider.first.value;
                        }else if(minCut&&!maxCut){
                            x = (position * rangeSlider.second.value/255.);
                            if(x<rangeSlider.first.value)
                                x = rangeSlider.first.value;
                        }else if(!minCut&&maxCut){
                            x = (position * (255-rangeSlider.first.value)/255.) + rangeSlider.first.value;
                            if(x>rangeSlider.second.value)
                                x = rangeSlider.second.value;
                        }else /*if(minOperation==CUT&&maxOperation==CUT)*/{
                            x = position;
                            if(x<rangeSlider.first.value)
                                x = rangeSlider.first.value;
                            if(x>rangeSlider.second.value)
                                x = rangeSlider.second.value;
                        }
                        x /= 255;
                        x *= (parent.width-2);
                    }

                    NumberAnimation {
                        target: positionRectangle
                        property: "position"
                        duration: 6000
                        easing.type: Easing.SineCurve
                        from: 0
                        to: 255
                        loops: Animation.Infinite
                        running: popup.visible
                    }
                }
            }            
            RangeSlider{
                enabled: UserManagment.currentUser.havePermission(Permission.CHANGE_MIN_MAX_MAPPING);
                id: rangeSlider
                from: 0
                to: 255
                stepSize: 1
                first.value: controlData?controlData.minValue:0;
                second.value: controlData?controlData.maxValue:255;
                first.onValueChanged: controlData.minValue = first.value
                second.onValueChanged: if(controlData)controlData.maxValue = second.value
                Text {
                    text: rangeSlider.first.value.toFixed(0)
                    anchors.bottom: rangeSlider.first.handle.top
                    anchors.horizontalCenter: rangeSlider.first.handle.horizontalCenter
                    anchors.bottomMargin: 2
                }
                Text {
                    text: rangeSlider.second.value.toFixed(0)
                    anchors.bottom: rangeSlider.second.handle.top
                    anchors.horizontalCenter: rangeSlider.second.handle.horizontalCenter
                    anchors.bottomMargin: 2
                }
            }

            ComboBox{
                hoverEnabled: true
                id: valueComboBox
                onHoveredChanged: hovered ? showInfoPopup.visible = hovered : hideTooltipTimer.start()
                ToolTip{
                    Timer{
                        id: hideTooltipTimer
                        interval: 500
                        onTriggered: showInfoPopup.visible = false;
                        repeat: false;
                    }
                    id: showInfoPopup
                    text: "The value of a DMX channel, e.g. the brightness of a lamp, can be determined in two ways. Either a default value is set, then programs run and then the result gets mapped to a different range. Alternatively, the value can simply be set to a specific value that overwrites alternative 1. 'Use value as default' describes alternative 1, 'Override with value' alternative 2."
                    delay: 1000
                    Component.onCompleted: contentItem.wrapMode = Label.Wrap
                }
                Component.onCompleted: {
                    valueComboBox.popup.onOpenedChanged.connect( () => {
                                                                    hideTooltipTimer.stop();
                                                                    showInfoPopup.visible = valueComboBox.popup.opened;
                                                                });
                }

                enabled: UserManagment.currentUser.havePermission(Permission.CHANGE_MIN_MAX_MAPPING);
                model: ["Use value as default","Override with value"]
                currentIndex: controlData?controlData.override:0
                onCurrentIndexChanged: {
                                        if(controlData)controlData.override = (currentIndex===1)
                }
                Layout.fillWidth: true
                //anchors.left: parent.left
                //anchors.right: parent.right
            }


            RowLayout{
                Label{
                    text:"Geräte : "
                }
                TextInputField{
                    Layout.fillWidth: true
                    id:search

                }
            }
            ListView{
                enabled: UserManagment.currentUser.havePermission(Permission.CHANGE_GROUP_DEVICES);
                width: 200
                height: 125
                clip:true
                model: controlData?controlData.groupModel:null
                id: deviceList
                property var regex: new RegExp(search.text,"i");
                delegate: CheckDelegate{
                    width: 200
                    text: modelData.name
                    visible: height !== 0
                    height: deviceList.regex.test(modelData.name) * implicitHeight
                    checked: use
                    clip:true
                    Behavior on height {
                        NumberAnimation{
                            duration: 250
                            easing.type: "OutQuad"
                        }
                    }

                    onCheckedChanged: use = checked

                }
            }
        }
        closePolicy: Popup.CloseOnPressOutside
    }
}
