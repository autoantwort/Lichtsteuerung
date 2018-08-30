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


        ProgramBlockEditor{
            anchors.left: page.right
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            programBlock: listView.currentItem.itemData;
        }



}
