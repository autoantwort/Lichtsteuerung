import QtQuick 2.11
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import custom.licht 1.0
import QtQuick.Controls.Material 2.2
import QtQuick.Dialogs 1.2

Item{
    id: root

    Item{
        clip: true
        anchors.top : parent.top
        anchors.bottom:  parent.bottom
        width: 300
        id: swipeView
        Page{

            id:tab1
            width: parent.width
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
                    text: modelData.name +  " (" + moduleTypeModel[modelData.type] + ')'
                    onClicked: {
                        listView.currentIndex = index;
                    }
                }
                highlight: Rectangle{
                    color: "blue"
                    opacity: 0.7
                }
                model: modulesModel

            }
            footer: RowLayout{
                Button{
                    Layout.preferredWidth: listView.width/2-5
                    Layout.leftMargin: 5
                    id: buttonAdd
                    text:"Add"
                    font.pixelSize: 15
                    onClicked: {ModelManager.addModule(); console.log("test");}
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
                        ModelManager.removeModule(index);
                    }
                }
            }
        }
    }

    GridLayout{
        anchors.left: swipeView.right
        anchors.leftMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 5
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        columns: 2
        Label{
            text: "Name:"
        }
        TextInputField{
            Layout.fillWidth: true
            enabled: listView.currentIndex !== -1
            text: listView.currentItem ? listView.currentItem.itemData.name : "Select one Module"
            onTextChanged: listView.currentItem.itemData.name = text;
        }

        Label{
            text: "Description:"
        }
        TextInputField{
            Layout.fillWidth: true
            enabled: listView.currentIndex !== -1
            text: listView.currentItem ? listView.currentItem.itemData.description : "Select one Module"
            onTextChanged: listView.currentItem.itemData.description = text;
        }

        Label{
            text: "Type:"
        }
        ComboBox{
            model: moduleTypeModel
            Layout.preferredWidth: 200
            currentIndex: listView.currentItem.itemData.type
            onCurrentIndexChanged: listView.currentItem.itemData.type = currentIndex
        }

        Label{
            text: "Properties:"
        }
        ListView{
            id: propertyView
            Layout.fillWidth: true
            anchors.bottomMargin: -20
            clip: true
            Layout.preferredHeight: Math.max(50,Math.min(model.rowCount() * 20,120))
            onModelChanged:{ for (var prop in model) {
                                print(prop += " (" + typeof(model[prop]) + ") = " + model[prop]);
                            }
            }
            model: listView.currentItem.itemData.properties
            delegate: ItemDelegate{
                id:delegate
                property var modelEntry : modelData
                text: modelData.name
                width: propertyView.width
                height: 20
                Component.onCompleted: console.log(modelEntry)
                onClicked: {
                    dialog.prop = modelEntry;
                    dialog.visible = true;
                }
                Button{
                    id:removeProp
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.right: addProp.left
                    anchors.topMargin: -4
                    anchors.bottomMargin: -4
                    Material.elevation: 0
                    width: height
                    onClicked: listView.currentItem.itemData.removeProperty(delegate.modelEntry)
                    Image {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        height: 24
                        width: 24
                        source: "icons/remove.svg"
                    }
                }
                Button{
                    id: addProp
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    anchors.topMargin: -4
                    anchors.bottomMargin: -4
                    width: height
                    Material.elevation: 0
                    onClicked: {
                        dialog.prop = listView.currentItem.itemData.createNewProperty();
                        dialog.visible = true;
                    }
                    Image {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        height: 24
                        width: 24
                        source: "icons/add.svg"
                    }
                }
            }
            Button{
                visible: propertyView.count === 0
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                width: height
                Material.elevation: 0
                onClicked: {
                    dialog.prop = listView.currentItem.itemData.createNewProperty();
                    dialog.visible = true;
                }
                Image {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    height: 24
                    width: 24
                    source: "icons/add.svg"
                }
            }
        }

        Label{
            text: "Code"
            font.underline: true
            Layout.columnSpan: 2
        }
        ScrollView{
            Layout.columnSpan: 2
            Layout.fillHeight: true
            Layout.fillWidth: true
            onHoveredChanged: if(!hovered)listView.currentItem.itemData.code = codeEditor.text
            TextArea{
                font.family: "Liberation Mono"
                font.pointSize: 10
                tabStopDistance: 16
                id: codeEditor
                selectByMouse: true
                text: listView.currentItem.itemData.code
                //onTextChanged: listView.currentItem.itemData.code = text

                CodeEditorHelper{
                    id:codeEditorHelper
                    module: listView.currentItem.itemData
                    document: codeEditor.textDocument
                    onInsertText: {
                        console.log(newText)
                        codeEditor.insert(codeEditor.cursorPosition,newText);
                        // Hack to display all new text, sometimes new text disappear
                        codeEditor.selectAll();
                        codeEditor.deselect();
                        codeEditor.cursorPosition = pos;
                    }
                    onInformation:{
                        informationDialog.text = text
                        informationDialog.visible = true;
                    }
                }
            }
        }
        Button{
            Layout.columnSpan: 2
            Layout.fillWidth: true
            text: "Compile, test and save"
            onClicked: codeEditorHelper.compile()
        }

    }


    MessageDialog{
        id: informationDialog
    }

    Dialog{
        property var prop;
        onPropChanged: print(prop.typ)
        modality: Qt.WindowModal
        title: "Add/Change Property"
        width:300
        id: dialog
        contentItem: Pane {
            GridLayout{
                anchors.fill: parent
                columns: 2
                Label{
                    text: "Name"
                }
                TextInputField{
                    id:name
                    Layout.fillWidth: true
                    text:dialog.prop.name
                    validator: RegExpValidator{
                        regExp: /[a-z][a-z_0-9]*$/i
                    }
                }
                Label{
                    text:"Beschreibung"
                }
                TextInputField{
                    id:besch
                    Layout.fillWidth: true
                    text:dialog.prop.description
                }
                Label{
                    text:"Typ"
                }
                ComboBox{
                    currentIndex: dialog.prop.type
                    id:type
                    model: modolePropertyTypeList
                    Layout.fillWidth: true
                    onCurrentIndexChanged: {
                        // "Int" << "Long" << "Float" << "Double" << "Bool" << "String";
                            minVal.enabled = currentIndex>=0 && currentIndex <=3;
                            maxVal.enabled = currentIndex>=0 && currentIndex <=3;
                        defaultVal.enabled = currentIndex>=0 && currentIndex <=4;
                        if(currentIndex === 4){
                            defaultVal.validator.top = 1;
                            defaultVal.validator.bottom = 0;
                        }else{
                            defaultVal.validator.top = 2^31;
                            defaultVal.validator.bottom = -2^31;
                        }
                    }
                }
                Label{
                    text:"min value"
                }
                TextInputField{
                    Layout.fillWidth: true
                    id: minVal
                    validator: IntValidator{}
                    text:type.currentIndex === 4 ? "0" : type.currentIndex === 5 ? "" : dialog.prop.minValue
                }

                Label{
                    text:"max value"
                }
                TextInputField{
                    Layout.fillWidth: true
                    id:maxVal
                    validator: IntValidator{}
                    text:type.currentIndex === 4 ? "1" : type.currentIndex === 5 ? "" : dialog.prop.maxValue
                }
                Label{
                    text:"default value"

                }
                TextInputField{
                    text: type.currentIndex !== 5 ? dialog.prop.defaultValue : ""
                    Layout.fillWidth: true
                    id:defaultVal
                    enabled: type.currentIndex !== 5
                    validator: IntValidator{
                        top:  dialog.prop.maxValue
                        bottom: dialog.prop.minValue
                    }
                }
                RowLayout{
                    Layout.columnSpan: 2
                    Button{
                        Layout.fillWidth:  true
                        text:"Abbrechen"
                        onClicked: dialog.visible = false
                    }
                    Button{
                        Layout.fillWidth:  true
                        text:"Übernehmen"
                        onClicked: {
                            dialog.visible = false
                            dialog.prop.name = name.text;
                            dialog.prop.description = besch.text;
                            dialog.prop.type = type.currentIndex;
                            if(minVal.text.length!==0)dialog.prop.minValue = minVal.text
                            if(maxVal.text.length!==0)dialog.prop.maxValue = maxVal.text
                            if(defaultVal.text.length!==0)dialog.prop.defaultValue = defaultVal.text
                        }
                    }
                }
            }
        }
    }
}
