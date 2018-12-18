import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtGraphicalEffects 1.0
import custom.licht 1.0
import QtQml 2.11

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
                    Text {
                        text: qsTr("Min")
                        font.underline: true
                        Layout.alignment: Qt.AlignHCenter
                    }
                    RadioButton{
                        text: "Remap"
                        checked: controlData?controlData.minOperation==1/*REMAP*/:false
                        onCheckedChanged: controlData.minOperation = 1
                    }
                    RadioButton{
                        text: "Cut"
                        checked: controlData?controlData.minOperation==0/*CUT*/:false
                        onCheckedChanged: controlData.minOperation = 0
                    }

                }
                ColumnLayout{
                    Text {
                        text: qsTr("Max")
                        font.underline: true
                        Layout.alignment: Qt.AlignHCenter
                    }
                    RadioButton{
                        text: "Remap"
                        checked: controlData?controlData.maxOperation==1/*REMAP*/:false
                        onCheckedChanged: controlData.maxOperation = 1
                    }
                    RadioButton{
                        text: "Cut"
                        checked: controlData?controlData.maxOperation==0/*CUT*/:false
                        onCheckedChanged: controlData.maxOperation = 0
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
                delegate: CheckDelegate{
                    width: 200
                    text: itemData.name
                    visible: height !== 0
                    height: itemData.name.startsWith(search.text,Qt.CaseInsensitive) * implicitHeight
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
