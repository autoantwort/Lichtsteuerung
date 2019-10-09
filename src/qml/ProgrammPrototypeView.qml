import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.3
import custom.licht 1.0
import "components"

ModelView{
    model:programmPrototypeModel
    id:modelView

    addButton.text: "Add Prototype"
    removeButton.text: "Remove Prototype"
    onAddClicked: dialog.visible = true
    onRemoveClicked: ModelManager.removeDmxProgramPrototype(remove);
    sortModel: ListModel{
        ListElement{
            name: "Creation Date"
            sortPropertyName: ""
        }
        ListElement{
            name: "Name"
            sortPropertyName: "name"
        }
        ListElement{
            name: "Device Prototype"
            sortPropertyName: "devicePrototype.name"
        }
    }
    ListView{
        clip:true
        Layout.column: 2
        Layout.columnSpan: 2
        Layout.row: 2
        Layout.fillHeight: true
        Layout.fillWidth: true
        header: Text{
            text:"Die einzelnen ChannelProgramme : "
            color: Material.foreground
            height:70
        }
        id : channelView
        spacing: 50
        delegate:
            ChannelProgrammEditor{
            layer.samples: 4
            clickRadius: 50
            antialiasing: true
            id:editor
            width: parent.width
            //onClicked: channelView.currentIndex = index
            height:100
            background: Rectangle{color:"lightgrey"}
            channelProgramm: modelData
            focus: true
            informationDisplayTextItem:Text{}
            RowLayout{
                height:50
                anchors.bottom: editor.top
                Label{
                    text:editor.channelProgramm.channel.name
                    font.bold: true
                    Layout.minimumWidth: 60
                }
                Label{
                    text:"time: "
                    visible: editor.currentTimePoint.hasCurrent
                }
                TextInputField{
                    Layout.minimumWidth: 30
                    text:editor.currentTimePoint.time
                    visible: editor.currentTimePoint.hasCurrent
                    onAccepted: editor.currentTimePoint.time = text
                }
                Label{
                    text:"value: "
                    visible: editor.currentTimePoint.hasCurrent
                }
                TextInputField{
                    Layout.minimumWidth: 30
                    text:editor.currentTimePoint.value
                    visible: editor.currentTimePoint.hasCurrent
                    onAccepted: editor.currentTimePoint.value = text
                    validator: IntValidator{bottom: 0; top:255}
                }
                Label{
                    text: "To Next:"
                    visible: editor.currentTimePoint.hasCurrent
                }
                ComboBox{
                    model: easingModel
                    property bool open: false
                    currentIndex: editor.currentTimePoint.curveToNext
                    visible: editor.currentTimePoint.hasCurrent
                    onDownChanged: {
                        open|=down;
                    }
                    onHighlighted: {
                        if(open)editor.currentTimePoint.curveToNext = index
                    }
                    onActivated: {
                        editor.currentTimePoint.curveToNext = index
                        open=false;
                    }
                }
            }
            onRightClick:{
                console.log(x + " "+y)
            }

        }

        model: modelView.currentModelData ? modelView.currentModelData.channelProgramms : null
    }



    Dialog{
        modality: Qt.WindowModal
        id:dialog
        title: "Hallo"
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
                        id:prototype
                        model: devicePrototypeModel
                        textRole: "display"
                    }
                    RowLayout{
                        Label{
                            id:nameLabel
                            text:"Name :"
                        }
                        TextInputField{
                            Layout.fillWidth: true
                            id:name
                        }
                    }
                    RowLayout{
                        Label{
                            text:"Description :"
                        }
                        TextInputField{
                            Layout.fillWidth: true
                            id:description
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
                            onClicked: {
                                if(name.text===""){
                                    name.underlineColor = "red";
                                }else{
                                    dialog.visible = false;
                                    ModelManager.addProgrammPrototype(prototype.currentIndex,name.text,description.text);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

}
