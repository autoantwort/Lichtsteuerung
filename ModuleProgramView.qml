import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import custom.licht 1.0
import QtQuick.Dialogs 1.2

Item{
    id: root

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
                    function update(){
                        if(propertiesView.currentIndex<0)
                            return;
                        var data = propertiesView.currentItem.itemData;

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
                    Layout.maximumHeight: 250
                    Layout.preferredHeight: 250
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
                    }

                }
            }
        }



}
