import QtQuick 2.9
import QtQuick.Controls 2.4
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
                    property var selected: ListView.isCurrentItem
                    width: parent.width
                    height: 60
                    onClicked: {
                        listView.currentIndex = index;
                    }

                    function makeCurrentItem(){
                        listView.currentIndex = index;
                    }

                    TextInputField{
                        id:input;
                        text: modelData.name
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 15
                        onTextChanged: modelData.name = text
                        color: parent.selected ? "white" : "black"
                        onFocusChanged: {
                            if(focus){
                                parent.makeCurrentItem();
                                forceActiveFocus("MouseFocusReason");
                            }
                        }
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
            spacing: 0
            visible: programEditor.programBlock!==null

            ProgramBlockEditor{
                Layout.fillHeight: true
                Layout.fillWidth: true
                id: programEditor
                programBlock: listView.currentItem.itemData;
                property var status: programBlock.status;
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
                // enum Status{Stopped=0, Running=1, Paused=2}

                RoundButton{
                    id: button_startStop
                    icon.source: programEditor.programBlock.status === 1 ? "qrc:icons/stop.svg" : programEditor.programBlock.status === 0 ? "qrc:icons/play.svg":"qrc:icons/replay.svg"
                    anchors.bottom: parent.bottom
                    anchors.leftMargin: 15
                    anchors.left: parent.left
                    checked: programEditor.programBlock.status === 1
                    onClicked: {
                        if(programEditor.programBlock.status === 0){
                            programEditor.programBlock.start();
                        } else if(programEditor.programBlock.status === 1){
                            programEditor.programBlock.stop();
                        } else if(programEditor.programBlock.status === 2){
                            programEditor.programBlock.restart();
                        }
                    }
                    ToolTip.visible: hovered
                    ToolTip.delay: 1000
                    ToolTip.text: programEditor.programBlock.status === 1 ? "Stop" : programEditor.programBlock.status === 0 ? "Play":"Replay"
                }
                RoundButton{
                    id: button_pauseResume
                    icon.source: programEditor.programBlock.status === 1 ? "qrc:icons/pause.svg" : "qrc:icons/resume.png"
                    visible: programEditor.programBlock.status !== 0;
                    checked: programEditor.programBlock.status === 1
                    anchors.bottom: parent.bottom
                    anchors.leftMargin: 15
                    anchors.left: button_startStop.right
                    onClicked: {
                        if(programEditor.programBlock.status === 1){
                            programEditor.programBlock.pause();
                        } else if(programEditor.programBlock.status === 2){
                            programEditor.programBlock.resume();
                        }
                    }
                    ToolTip.visible: hovered
                    ToolTip.delay: 1000
                    ToolTip.text: programEditor.programBlock.status === 1 ? "Pause" : "Resume"
                }

                RoundButton{
                    id:button_add_entry
                    text: "Add Entry"
                    anchors.bottom: parent.bottom
                    anchors.left: button_pauseResume.right
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






            Rectangle{
                color: "lightgrey"
                Layout.fillHeight: true
                width: 1
            }
            ColumnLayout{
                Layout.fillHeight: true
                Layout.preferredWidth: 200
                Layout.maximumWidth: 200
                visible: programEditor.showProperties

                spacing: 0

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
                    Layout.fillWidth: true
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
                    function intHandler() {
                        propertiesView.currentItem.itemData.value = spinBox.value;
                    }
                    function doubleHandler() {
                        propertiesView.currentItem.itemData.value = spinBox.value/1000;
                    }
                    function update(){
                        if(propertiesView.currentIndex<0)
                            return;
                        var data = propertiesView.currentItem.itemData;
                        if(data.type>=0&&data.type<=3){
                            spinBox.onValueChanged.disconnect(intHandler);
                            spinBox.onValueChanged.disconnect(doubleHandler);
                            if(data.type == 0 || data.type == 1){ // int
                                spinBox.textFromValue = (value,locale) => Number(value).toLocaleString(locale,'f',0);
                                spinBox.valueFromText = (text,locale) => Number.fromLocaleString(locale,text);
                                spinBox.from = data.minValue;
                                spinBox.to = data.maxValue;
                                spinBox.value = data.value;
                                spinBox.stepSize = 1;
                                spinBoxValidator.decimals = 0;
                                spinBox.onValueChanged.connect(intHandler);
                            }else{ //float
                                spinBox.textFromValue = (value,locale) => Number(value/1000).toLocaleString(locale,'f',3);
                                spinBox.valueFromText = (text,locale) => Number.fromLocaleString(locale,text) * 1000;
                                spinBoxValidator.decimals = 3
                                spinBox.from = data.minValue * 1000;
                                spinBox.to = data.maxValue * 1000;
                                spinBox.value = data.value * 1000;
                                spinBox.stepSize = 50;
                                spinBox.onValueChanged.connect(doubleHandler);
                            }
                            spinBoxValidator.bottom = data.minValue;
                            spinBoxValidator.top = data.maxValue;
                            spinBox.visible = true;
                        }else{
                            if(data.type===4){
                                checkBox.checked = data.value
                            }

                            spinBox.visible = false;
                        }
                        checkBox.visible = data.type === 4;

                    }
                }


                Rectangle{
                    color: "lightgrey"
                    Layout.preferredHeight: 1
                    Layout.fillWidth: true
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
                    SpinBox{
                        id:spinBox
                        validator: DoubleValidator{
                            id: spinBoxValidator
                        }
                        editable: true
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
