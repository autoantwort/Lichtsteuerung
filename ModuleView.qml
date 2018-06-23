import QtQuick 2.11
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import custom.licht 1.0

Item{
    id: root

    Item{
        clip: true
        anchors.top : parent.top
        anchors.bottom:  parent.bottom
        width: 300
        //interactive: false
        id: swipeView
        /*function switchTab(){
            if(tab1.x===0){
                tab1.x = -width;
                tab2.x = 0;
            }else{
                tab1.x = 0;
                tab2.x = width;
            }
        }*/
        Page{

            id:tab1
            Behavior on x {
                NumberAnimation { easing.type: Easing.InOutCubic; easing.amplitude: 9; easing.period: 50.0; duration: 200 }
            }
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
//                        for (var prop in moduleTypeModel) {
//                                    print(prop += " (" + typeof(moduleTypeModel[prop]) + ") = " + moduleTypeModel[prop]);
//                                }
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
                }
            }
        }
        /*Page{
            x: 9999 // far far away, nobody should see this Component at Lunch time
            id:tab2
            Behavior on x {
                NumberAnimation { easing.type: Easing.InOutCubic; easing.amplitude: 9; easing.period: 50.0; duration: 200 }
            }
            width: parent.width
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            //height: 500
            //width: 300
            //clip: true
            //anchors.fill: parent
            header: ItemDelegate{
                id:backDelegate
                text: "Back"
                onClicked: swipeView.switchTab()
            }
            ListView{
                id:listView2
                delegate: ItemDelegate{
                    width: parent.width
                    text:  " test : " + itemData.name +"("+itemData.description+")"
                    onClicked: listView.currentIndex = index

                }
                highlight: Rectangle{
                    color: "blue"
                    opacity: 0.7
                }
                model: devicePrototypeModel

            }
            footer:  RowLayout{
                Button{
                    Layout.preferredWidth: listView.width/2 -5
                    Layout.leftMargin: 5
                    id: buttonAdd2
                    text:"Add"
                    font.pixelSize: 15
                }
                Button{
                    Layout.preferredWidth: listView.width/2 -10
                    Layout.rightMargin: 10
                    id: buttonRemove2
                    text:"Remove"
                    font.pixelSize: 15
                }
            }
        }*/
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
            delegate: ItemDelegate{

                text: name
                width: propertyView.width
                height: 20
            }
            Layout.preferredHeight: 50
            model: ListModel{
                ListElement{
                    name: "Prop1"
                }
                ListElement{
                    name: "Prop2"
                }

                ListElement{
                    name: "Prop2"
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



}
