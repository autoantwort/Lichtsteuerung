import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import custom.licht 1.0
import QtQuick.Dialogs 1.2

Item{
    id: root
    SwipeView.onIsCurrentItemChanged: {
        programEditor.showProperties = false;
    }

        Page{
            width: 300
            id:page
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            clip: true
            ListView{
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right:  parent.right
                anchors.bottom: parent.bottom
                id:listView
                delegate: ItemDelegate{
                    property var itemData : modelData
                    width: parent.width
                    height: 60
                    text: modelData.name
                    onClicked: {
                        listView.currentIndex = index;
                    }
                }
                highlight: Rectangle{
                    color: "blue"
                    opacity: 0.7
                }
                model: programBlocksModel

            }
            footer: RowLayout{
                Button{
                    Layout.preferredWidth: listView.width/2-5
                    Layout.leftMargin: 5
                    id: buttonAdd
                    text:"Add"
                    font.pixelSize: 15
                    onClicked: {ModelManager.addProgramBlock()}
                }
                Button{
                    Layout.preferredWidth: listView.width/2-10
                    Layout.rightMargin: 10
                    id: buttonRemove
                    text:"Remove"
                    font.pixelSize: 15
                    onClicked: {
                        var index = listView.currentIndex;
                        if(index === 0){
                            listView.currentIndex = 1;
                        }else{
                            listView.currentIndex = index - 1;
                        }
                        ModelManager.removeProgramBlock(index);
                    }
                }
            }
        }


        RowLayout{
            anchors.left: page.right
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            ProgramBlockEditor{
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                Layout.fillWidth: true
                id: programEditor
                programBlock: listView.currentItem.itemData;
                onShowPropertiesChanged: propertiesView.update()
                onOpenRightClickEntry: {
                    rightClickMenu.x = x;
                    rightClickMenu.y = y;
                    rightClickMenu.open();
                }
                onAskToAddConnection: {
                    popup_addConnectionAsk.outputName = from
                    popup_addConnectionAsk.inputName = to
                    popup_addConnectionAsk.visible = true;
                }
                RoundButton{
                    id: button_run
                     checkable: true
                     checked: programEditor.run
                     onCheckedChanged: programEditor.run = checked
                     text: programEditor.run ? "Stop" : "Run"
                     width: 70
                     anchors.bottom: parent.bottom
                     anchors.left: parent.left
                     anchors.leftMargin: 15
                 }
                RoundButton{
                    id:button_add_entry
                    text: "Add Entry"
                    anchors.bottom: parent.bottom
                    anchors.left: button_run.right
                    anchors.leftMargin: 15
                    onClicked: {
                        programEditor.updatePossibleEntries();
                        addEntry.visible = true;
                    }
                }
                RoundButton{
                    id:button_add_connection
                    text: "Add Connection"
                    anchors.bottom: parent.bottom
                    anchors.left: button_add_entry.right
                    anchors.leftMargin: 15
                    onClicked: {
                        programEditor.updateInputOutputModels();
                        popup_addConnection.visible = true;
                    }
                }
                RoundButton{
                    id:button_help
                    text: "Help"
                    anchors.bottom: parent.bottom
                    anchors.left: button_add_connection.right
                    anchors.leftMargin: 15
                    onClicked: {
                        popup_help.visible = true;
                    }
                }
                Menu{
                    id:rightClickMenu
                    width: 270
                    MenuItem{
                        id:firstItem
                        text: "Remove incoming Connections"
                        onClicked: programEditor.removeIncomingConnections();
                    }
                    MenuItem{
                        text: "Remove entry"
                        onClicked: programEditor.removeEntry()
                    }

                }
            }






            ColumnLayout{
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                Layout.preferredWidth: 200
                Layout.maximumWidth: 200
                visible: programEditor.showProperties

                spacing: 0
                Rectangle{
                    id:line
                    color: "lightgrey"
                    width: 1
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.bottom: parent.bottom
                }

                id: propertyPane
                Label{
                    id:label
                    Layout.margins: 8
                    font.pointSize: 16
                    text:"Properties"
                }
                Rectangle{

                    color: "lightgrey"
                    Layout.preferredHeight: 1
                    anchors.left: parent.left
                    anchors.right: parent.right
                }
                ListView{
                    clip:true
                    Layout.preferredHeight: 200
                    Layout.fillHeight: true
                    Layout.preferredWidth: parent.width
                    id: propertiesView
                    model: programEditor.propertyInformationModel
                    delegate: ItemDelegate{
                        property var itemData : modelData
                        width: parent.width
                        text: modelData.name
                        onClicked: propertiesView.currentIndex = index
                    }
                    highlightFollowsCurrentItem: true
                    highlight: Rectangle{
                        color: "lightgrey"
                    }
                    onCurrentItemChanged: update()
                    function update(){
                        if(propertiesView.currentIndex<0)
                            return;
                        var data = propertiesView.currentItem.itemData;
                        print("type : " + data.type)
                        if(data.type>=0&&data.type<=3){
                            slider.visible = true;
                            // Wenn die folgenden zwei zeilen fehlen, wird der value beim setzten auf die grenzen gesetzt und dann verändert
                            slider.to = 9999999999;
                            slider.from = -9999999999999;
                            slider.value = data.value;
                            slider.from = data.minValue;
                            slider.to = data.maxValue;
                            slider.stepSize = (data.type==0||data.type==1)?1:0;
                            slider.snapMode = (data.type==0||data.type==1)?Slider.SnapAlways:Slider.NoSnap;
                            print("stepSize : " + slider.stepSize)
                        }else{
                            if(data.type===4){
                                checkBox.checked = data.value
                            }

                            slider.visible = false;
                        }
                        checkBox.visible = data.type === 4;

                    }
                }


                Rectangle{
                    id:hline
                    color: "lightgrey"
                    Layout.preferredHeight: 1
                    anchors.left: parent.left
                    anchors.right: parent.right
                }


                ColumnLayout{
                    Label{
                        Layout.margins: 5
                        Layout.preferredHeight: contentHeight
                        Layout.preferredWidth: 190
                        clip:true
                        wrapMode: Text.WordWrap
                        text:propertiesView.currentItem.itemData.description
                    }
                    Slider{
                        id:slider
                        onValueChanged: {
                            propertiesView.currentItem.itemData.value = value;
                            print(propertiesView.currentItem.itemData.name + " set value to " + value)
                        }
                        Text{
                            text: slider.stepSize===0?slider.value.toFixed(2):slider.value.toFixed(0);
                            anchors.bottom: slider.handle.top
                            anchors.bottomMargin: 7
                            anchors.horizontalCenter: slider.handle.horizontalCenter
                        }
                    }
                    CheckBox{
                        id:checkBox
                    }
                    Rectangle{
                        height: 1
                        Layout.preferredWidth: 190
                        Layout.leftMargin: 5
                        color: "lightgrey"
                    }
                    CheckBox{
                        id:forward
                        text: "Forward property to user."
                    }
                    Label{
                        text: "Propertyname for the user:"
                        Layout.leftMargin: 10
                        color: forward.checked?"black":"grey"
                    }
                    TextInputField{
                        Layout.preferredWidth: 190
                        Layout.leftMargin: 10
                        Layout.bottomMargin: 10
                        enabled: forward.checked
                        color: forward.checked?"black":"grey"
                    }

                }
            }
        }

        Popup{
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            //modality: Qt.WindowModal
            id:addEntry
            //title: "Choose entry"
            width:300
            contentItem: RowLayout {
                anchors.left: addEntry.left;
                anchors.right: addEntry.right;
                Layout.fillWidth: true
                    ColumnLayout{
                        Layout.fillWidth: true
                        ComboBox{
                            textRole: "display"
                            id:comboBoxEntry
                            Layout.fillWidth: true
                            model: programEditor.possibleEntryModel
                        }
                        RowLayout{
                            Label{
                                text:"Length/Size"
                            }
                            TextInputField{
                                Layout.fillWidth: true
                                id:text_size
                                text:"50"
                                validator: IntValidator{
                                    bottom: 1
                                    top: 1000
                                }
                            }
                        }

                        RowLayout{
                            Button{
                                Layout.fillWidth: true
                                text:"Cancel"
                                onClicked: addEntry.visible = false
                            }
                            Button{
                                Layout.fillWidth: true
                                text:"Create"
                                enabled: comboBoxEntry.currentIndex>=0 && text_size.text.length>0
                                onClicked: {
                                    addEntry.visible = false;
                                    programEditor.addEntry(comboBoxEntry.currentIndex,text_size.text)
                                }
                            }
                        }
                    }

            }
        }

        Popup{
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            //modality: Qt.WindowModal
            id:popup_addConnection
            //title: "Choose entry"
            width:300
            contentItem: RowLayout {
                anchors.left: popup_addConnection.left;
                anchors.right: popup_addConnection.right;
                Layout.fillWidth: true
                    ColumnLayout{
                        Layout.fillWidth: true
                        ComboBox{
                            textRole: "display"
                            id:comboBoxInput
                            Layout.fillWidth: true
                            model: programEditor.inputDataConsumerModel
                        }
                        RowLayout{
                            Label{
                                text:"Length/Size"
                            }
                            TextInputField{
                                Layout.fillWidth: true
                                id:connection_length
                                text:"50"
                                validator: IntValidator{
                                    bottom: 0
                                    top: 1000
                                }
                            }
                        }
                        ComboBox{
                            textRole: "display"
                            id:comboBoxOutput
                            Layout.fillWidth: true
                            model: programEditor.outputDataProducerModel
                        }
                        RowLayout{
                            Label{
                                text:"Startindex"
                            }
                            TextInputField{
                                Layout.fillWidth: true
                                id:connection_startIndex
                                text:"0"
                                validator: IntValidator{
                                    bottom: 0
                                    top: 1000
                                }
                            }
                        }

                        RowLayout{
                            Button{
                                Layout.fillWidth: true
                                text:"Cancel"
                                onClicked: popup_addConnection.visible = false
                            }
                            Button{
                                Layout.fillWidth: true
                                text:"Create"
                                enabled: comboBoxInput.currentIndex>=0 && connection_startIndex.text.length>0 && comboBoxOutput.currentIndex >= 0 && connection_length.text.length >= 0
                                onClicked: {
                                    popup_addConnection.visible = false;
                                    programEditor.addConnection(comboBoxInput.currentIndex,connection_length.text,comboBoxOutput.currentIndex,connection_startIndex.text);
                                }
                            }
                        }
                    }

            }
        }

        Popup{
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            id: popup_help
            contentItem: Label{
                text:"Use drag and drop:<ul><li>Without Modifier to add a normal connection</li><li>With <i>Alt</i> to add Connections from a Entrie lower as the target</li><li>With <i>Shift</i> to move entries</li><li>With <i>Str</i> to move an entry temporarily</li></ul>"
                wrapMode: "WordWrap"
                width: 300
            }
            width: 500
        }

        Popup{
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            id: popup_addConnectionAsk
            property string outputName
            property string inputName
            contentItem: ColumnLayout{
                    Label{
                        text:"Add a connection from the OutputDataProducer " + popup_addConnectionAsk.outputName + " to the InputDataConsumer " + popup_addConnectionAsk.inputName + "."
                        Layout.fillWidth: true
                        wrapMode: "WordWrap"
                    }
                    Label{
                        Layout.fillWidth: true
                        text: "         Output Units will be taken from the OutputDataProducer"
                        wrapMode: "WordWrap"
                        TextInputField{
                            id: textInput_connectionWidth
                            width: 35
                            text:"50"
                            validator: IntValidator{
                                bottom: 1
                                top: 9999
                            }
                            anchors.left: parent.left
                            anchors.top: parent.top
                            font.pixelSize: parent.font.pixelSize
                        }
                    }


                    Label{
                        Layout.fillWidth: true
                        text: "         is the index of the first Output Units in the array of output units of the OutputDataProducer"
                        wrapMode: "WordWrap"
                        TextInputField{
                            text:"0"
                            id: textInput_connectionStartIndex
                            width: 35
                            validator: IntValidator{
                                bottom: 0
                                top: 9999
                            }
                            anchors.left: parent.left
                            anchors.top: parent.top
                            font.pixelSize: parent.font.pixelSize
                        }
                    }

                RowLayout{
                    Button{
                        Layout.fillWidth: true
                        text:"Cancel"
                        onClicked: popup_addConnectionAsk.visible = false
                    }
                    Button{
                        Layout.fillWidth: true
                        text:"Create"
                        enabled: textInput_connectionWidth.text.length>0 && textInput_connectionStartIndex.text.length > 0
                        onClicked: {
                            popup_addConnectionAsk.visible = false;
                            programEditor.addConnection(textInput_connectionWidth.text,textInput_connectionStartIndex.text);
                        }
                    }
                }
            }
            contentWidth: 300
            contentHeight: 220 // the normal implicitHeight is wrong
        }


}
