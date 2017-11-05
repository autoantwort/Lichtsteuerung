import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2

ModelView{
    id: deviceModelView
    model:deviceModel
    rows: 7
    onAddClicked:dialog.visible = true
    Text{
        Layout.row: 2
        Layout.column: 2
        text:"startDMXChannel:"
        Layout.rightMargin: 20
        font.pixelSize: 15
    }
    TextInputField{
        Layout.row: 2
        Layout.column: 3
        text: parent.currentItem.data.startDMXChannel
        validator: IntValidator{
            bottom: 0
            top:512
        }
        onTextChanged: parent.currentItem.data.startDMXChannel = text.length?text:0
    }
    Text{
        Layout.row: 3
        Layout.column: 2
        text:"DevicePrototype:"
        Layout.rightMargin: 20
        font.pixelSize: 15
    }
    Text{
        Layout.row: 3
        Layout.column: 3
        text:parent.currentItem.data.prototype.name
        font.pixelSize: 15
        TextUnderline{
            extendetWidth:1
            color:"lightgrey"
        }
    }
    Text{
        Layout.row: 4
        Layout.column: 2
        text:"Position:"
        Layout.rightMargin: 20
        font.pixelSize: 15
    }
    RowLayout{
        Layout.row: 4
        Layout.column: 3
        Text{
            text:"x:"
            font.pixelSize: 15
        }
        TextInputField{
            Layout.minimumWidth: 50
            text:deviceModelView.currentItem.data.position.x
            validator: IntValidator{}
            onTextChanged: deviceModelView.currentItem.data.position.x = text.length?text:0
        }
        Text{
            Layout.leftMargin: 10
            text:"y:"
            font.pixelSize: 15
        }
        TextInputField{
            text:deviceModelView.currentItem.data.position.y
            validator: IntValidator{}
            onTextChanged: deviceModelView.currentItem.data.position.y = text.length?text:0
        }
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
                        Label{
                            text:"Start DMX Channel :"
                        }
                        TextInputField{
                            Layout.fillWidth: true
                            id:dmx
                            validator: IntValidator{
                            bottom: 0
                            top:1024
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
                            onClicked: {
                                if(name.text===""){
                                    name.underlineColor = "red";
                                }else{
                                    dialog.visible = false;
                                    ModelManager.addDevice(prototype.currentIndex,dmx.text,name.text,description.text);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

}
