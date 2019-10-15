import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import "../components"

ModelView{
    model:programmModel
    id:modelView
    rows: 9
    rowSpacing: 2
    addButton.text: "Add Programm"
    removeButton.text: "Remove Programm"
    onAddClicked: ModelManager.addProgramm("New Programm");
    onRemoveClicked: ModelManager.removeDmxProgram(remove);
    sortModel: ListModel{
        ListElement{
            name: "Creation Date"
            sortPropertyName: ""
        }
        ListElement{
            name: "Name"
            sortPropertyName: "name"
        }
    }
    Label{
        text: "Speed : "
    }
    TextInputField{
        text: modelView.currentModelData ? modelView.currentModelData.speed:""
        validator: DoubleValidator{}
        onTextChanged: if(modelView.currentModelData) modelView.currentModelData.speed = text
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
        checked: modelView.currentModelData?modelView.currentModelData.timeDistortion.enabled:false
        onCheckStateChanged: if(modelView.currentModelData) modelView.currentModelData.timeDistortion.enabled = checked
    }
    Label{
        Layout.leftMargin: 6
        text:"Intervall:"
    }
    TextInputField{
        Layout.fillWidth: true
        validator: DoubleValidator{}
        text: modelView.currentModelData ? modelView.currentModelData.timeDistortion.intervall:""
        onTextChanged: {if(modelView.currentModelData!==null)modelView.currentModelData.timeDistortion.intervall = text;}
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
        currentIndex: modelView.currentModelData?modelView.currentModelData.timeDistortion.distortionCurve.type:0
        onCurrentIndexChanged: {if(modelView.currentModelData!==null)modelView.currentModelData.timeDistortion.distortionCurve.type = currentIndex;}
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
        model: modelView.currentModelData ? modelView.currentModelData.programs:null
        highlight: Rectangle{
            color: "blue"
            opacity: 0.7
        }
    }


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
            onClicked: modelView.currentModelData.removeDeviceProgramm(channelView.currentIndex)
        }
    }


    Popup{
        modal: true
        id: dialog
        width: 300
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        contentItem: ColumnLayout{
            spacing: 10

            Label{
                text: "Add Device/ProgramPrototype"
                font.bold: true
                font.pointSize: 12
            }

            ComboBox{
                Layout.fillWidth: true
                id:device
                model: deviceModel
                textRole: "display"
            }


            ComboBox{
                Layout.fillWidth: true
                id: programmPrototype
                model: programmPrototypeModel
                textRole: "modelData"
                function hasProgrammPrototypeSelected(){
                    return programmPrototypeModel.data(programmPrototypeModel.index(programmPrototype.currentIndex,0),-1).devicePrototype === deviceModel.data(deviceModel.index(device.currentIndex,0),-1).prototype;
                }
                displayText: hasProgrammPrototypeSelected() ? programmPrototypeModel.data(programmPrototypeModel.index(currentIndex,0), -1).name:"Keine Auswahl getroffen";
                delegate: MenuItem {
                    width: parent.width
                    text: modelData.name
                    Material.foreground: programmPrototype.currentIndex === index ? parent.Material.accent : parent.Material.foreground
                    highlighted: programmPrototype.highlightedIndex === index
                    height: device.model.data(device.model.index(device.currentIndex,0),-1).prototype === modelData.devicePrototype ? implicitHeight : 0
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

                        modelView.currentModelData.addDeviceProgramm(deviceModel.data(deviceModel.index(device.currentIndex,0),-1),programmPrototypeModel.data(programmPrototypeModel.index(programmPrototype.currentIndex,0),-1),name.text);

                    }
                }
            }
        } // contentItem: ColumnLayout
    } // Popup

}
