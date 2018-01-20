import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import custom.licht 1.0

ModelView{
    model:programmPrototypeModel
    id:modelView

    addButton.text: "Add Prototype"
    removeButton.text: "Remove Prototype"

    ListView{
        clip:true
        Layout.column: 2
        Layout.columnSpan: 2
        Layout.row: 2
        Layout.fillHeight: true
        Layout.fillWidth: true
        header: Text{
            text:"Die einzelnen ChannelProgramme : "
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
                    currentIndex: editor.currentTimePoint.curveToNext
                    visible: editor.currentTimePoint.hasCurrent
                    onHighlightedIndexChanged: {editor.currentTimePoint.curveToNext = highlightedIndex
                        console.log("selected")
                    }
                }
            }

        }

        model: modelView.currentItem.data.channelProgramms
    }


    onAddClicked: dialog.visible = true

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
