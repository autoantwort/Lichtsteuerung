import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2

ModelView{
    model:programmModel
    id:modelView
    rows: 9
    rowSpacing: 2
    addButton.text: "Add Programm"
    removeButton.text: "Remove Programm"

    Label{
        text: "Speed : "
    }
    TextInputField{
        text: modelView.currentItem?modelView.currentItem.data.speed:""
        validator: DoubleValidator{}
        onTextChanged: modelView.currentItem.data.speed = text
    }
    Label{
        text: "Time Distortion:"
        font.underline: true
    }
    Label{}
    Label{
        Layout.leftMargin: 6
        id:beginTD
        text:"Enabled:"
    }
    CheckBox{
        id:topTD
        Layout.preferredHeight: 25
        Layout.leftMargin: -10
        checked: modelView.currentItem?modelView.currentItem.data.timeDistortion.enabled:false
        onCheckStateChanged: modelView.currentItem.data.timeDistortion.enabled = checked
    }
    Label{
        Layout.leftMargin: 6
        text:"Intervall:"
    }
    TextInputField{
        Layout.fillWidth: true
        validator: DoubleValidator{}
        text: modelView.currentItem?modelView.currentItem.data.timeDistortion.intervall:""
        onTextChanged: {if(modelView.currentItem!==null)modelView.currentItem.data.timeDistortion.intervall = text;}
    }
    Label{
        Layout.leftMargin: 6
        text: "Distortion Curve:"
    }
    ComboBox{
        Layout.fillWidth: true
        Layout.rightMargin: 5
        id:endTD
        model:easingModel
        currentIndex: modelView.currentItem?modelView.currentItem.data.timeDistortion.distortionCurve.type:0
        onCurrentIndexChanged: {if(modelView.currentItem!==null)modelView.currentItem.data.timeDistortion.distortionCurve.type = currentIndex;}
    }
    Rectangle{
        anchors.top: topTD.top
        anchors.bottom: endTD.bottom
        anchors.right: beginTD.left
        color: "lightgreen"
        width: 2
        anchors.margins: 2
        anchors.leftMargin: 6
    }

    ListView{
        clip:true
        Layout.column: 2
        Layout.columnSpan: 2
        Layout.row: 7
        Layout.fillHeight: true
        Layout.fillWidth: true

        id : channelView

        delegate: ItemDelegate{
            anchors.margins: 5
            clip : true
            width: parent.width
            height: ListView.isCurrentItem ? gridLayout.height+20:defaultHeightReference.height+20
            Behavior on height {
                NumberAnimation{
                    duration: 500
                    easing.type: Easing.OutQuart
                }
            }
            onClicked: channelView.currentIndex = index
            GridLayout{
                anchors.leftMargin: 10
                x:10
                y:10
                rowSpacing:10
                Layout.margins: 5
                columns: 2
                id:gridLayout
                Label{
                    id:defaultHeightReference
                    text:"Gerät : "
                }
                Label{
                    text: modelData.device.name
                }
                Label{
                    text:"Programm: "
                }
                Label{
                    text: modelData.programmPrototype.name
                }
                Label{
                    text:"Offset : "
                }
                TextInputField{
                    text : modelData.offset
                    onTextChanged: modelData.offset = text
                    validator: DoubleValidator{}
                }
                Label{
                    text:"Speed : "
                }
                TextInputField{
                    text : modelData.speed
                    onTextChanged: modelData.speed = text
                    validator: DoubleValidator{}
                }

            }
        }
        model: modelView.currentItem?modelView.currentItem.data.programs:null
        highlight: Rectangle{
            color: "blue"
            opacity: 0.7
        }
    }

    onAddClicked: ModelManager.addProgramm("New Programm");

    RowLayout{
        Layout.row: 8
        Layout.column: 2
        Layout.columnSpan: 2
        Layout.fillWidth: true
        Button{
            Layout.fillWidth: true
            text:"Add Programmprototype"
            font.pixelSize: 15
            onClicked: dialog.visible=true
        }
        Button{
            Layout.fillWidth: true
            text:"Remove Programmprototype"
            font.pixelSize: 15
            onClicked: modelView.currentItem.data.removeDeviceProgramm(channelView.currentIndex)
        }
    }


    Dialog{
        modality: Qt.WindowModal
        id:dialog
        title: "Ein neues DeviceProgramm hinzufügen"
        width:300
        contentItem: RowLayout {
            Pane{
                Layout.fillWidth: true
                ColumnLayout{
                    anchors.left: parent.left
                    anchors.right: parent.right
                    spacing: 10
                    ComboBox{
                        Layout.fillWidth: true
                        id:device
                        model: deviceModel
                        textRole: "display"
                    }


                    ComboBox{
                        Layout.fillWidth: true
                        id:programmPrototype
                        model: programmPrototypeModel
                        textRole: "itemData"
                        function hasProgrammPrototypeSelected(){
                            return programmPrototypeModel.data(programmPrototypeModel.index(programmPrototype.currentIndex,0),-1).devicePrototype === deviceModel.data(deviceModel.index(device.currentIndex,0),-1).prototype;
                        }
                        displayText: hasProgrammPrototypeSelected() ? programmPrototypeModel.data(programmPrototypeModel.index(currentIndex,0)):"Keine Auswahl getroffen";
                        delegate: ItemDelegate{
                            text: itemData.name
                            width: programmPrototype.popup.contentItem.width
                            visible: device.model.data(device.model.index(device.currentIndex,0),-1).prototype === itemData.devicePrototype
                        }
                    }
                    RowLayout{
                        Label{
                            id:nameLabel
                            text:"Time Offset :"
                        }
                        TextInputField{
                            Layout.fillWidth: true
                            id:name
                            text:"0"
                            validator: DoubleValidator{
                            }
                        }
                    }
                    RowLayout{
                        Button{
                            Layout.fillWidth: true
                            text:"Abbrechen"
                            onClicked: dialog.visible = false
                        }
                        Button{
                            Layout.fillWidth: true
                            text:"Erzeugen"
                            enabled: programmPrototypeModel.data(programmPrototypeModel.index(programmPrototype.currentIndex,0),-1).devicePrototype === deviceModel.data(deviceModel.index(device.currentIndex,0),-1).prototype
                            onClicked: {
                                dialog.visible = false;

                                modelView.currentItem.data.addDeviceProgramm(deviceModel.data(deviceModel.index(device.currentIndex,0),-1),programmPrototypeModel.data(programmPrototypeModel.index(programmPrototype.currentIndex,0),-1),name.text);

                            }
                        }
                    }
                }
            }
        }
    }

}
