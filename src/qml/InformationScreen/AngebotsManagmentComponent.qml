import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import "../"
import "../components"

Item {
    property alias model : angebote.model;
    ColumnLayout{
        id: columnLayout
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        width: 200
        ListView{
            property var currentData: currentItem ? model.get(currentIndex) : null
            property bool isCurrentlySection: currentData ? currentData.section === model.null_section : false
            property bool isCurrentlyEntry: currentData ? currentData.section !== model.null_section : false
            Layout.leftMargin: -5
            id: angebote
            delegate: ItemDelegate{
                id: delegate
                onClicked: angebote.currentIndex = index;
                text: section === angebote.model.null_section ? name : "     " + name
                font.pixelSize: section === angebote.model.null_section ? 16 : 13
                width: angebote.width
                Component.onCompleted: {
                    for(var i in contentItem)
                        console.log(i,contentItem[i])
                    contentItem.leftPadding = -11
                    contentItem.color = Qt.binding(function(){
                        if(offered){
                            if(section !== angebote.model.null_section){
                                console.log( "parent section",section.name," is offered : " , section.offered)
                                return section.offered ? "black" : "grey";
                            }else{
                                return "black"
                            }
                        }else{
                            return "grey"
                        }
                    })
                }

                Binding{
                    target: delegate.contentItem
                    property: "color"
                    value: delegate.isoffered ? "black" : "grey"
                }
            }
            highlight: Rectangle{
                color: "lightblue"
            }

            clip: true
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
        Button{
            id: deleteButton
            text: "remove"
            onClicked: angebote.model.removeEntry(angebote.currentIndex);
            Layout.fillWidth: true
            Layout.margins: 5
            enabled: angebote.currentIndex >= 0 && (angebote.isCurrentlyEntry || angebote.isCurrentlySection&&(angebote.model.count - 1 === angebote.currentIndex || angebote.model.get(angebote.currentIndex+1).section === angebote.model.null_section))
        }
    }

    GridLayout{
        anchors.left: columnLayout.right
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.leftMargin: 5
        columns: 2
        rowSpacing: 10
        columnSpacing: 10
        Label{
            text: "Name"
        }
        TextInputField{
            Layout.fillWidth: true
            text: angebote.currentData ? angebote.currentData.name : "No Entry selected";
            enabled: angebote.currentItem
            onTextChanged: {
                if(angebote.currentData){
                    angebote.currentData.name = text;
                }
            }
        }
        Label{
            text: "Description"
        }
        TextInputField{
            Layout.fillWidth: true
            text: angebote.isCurrentlyEntry ? angebote.currentData.description : angebote.isCurrentlySection ? "No description possible":"No Entry selected";
            enabled: angebote.isCurrentlyEntry
            onTextChanged: if(angebote.isCurrentlyEntry)angebote.currentData.description = text
        }
        Label{
            text: "Price"
        }
        TextInputField{
            Layout.fillWidth: true
            text: angebote.isCurrentlyEntry ? angebote.currentData.price : "Can not have a price"
            enabled: angebote.isCurrentlyEntry
            onTextChanged: if(angebote.currentItem)angebote.currentData.price = text
        }
        Label{
            text: "Offered"
        }
        CheckBox{
            enabled: angebote.currentItem
            checked: angebote.currentData ? angebote.currentData.offered : false
            onCheckedChanged: if(angebote.currentData)angebote.model.setProperty(angebote.currentIndex,"offered",checked)
            Layout.margins: -10
            Text {
                visible: angebote.currentData ? angebote.currentData.section !== angebote.model.null_section && !angebote.currentData.section.offered : false
                anchors.left: parent.right
                anchors.baseline: parent.baseline
                text: "The section is not offered, so this Entry will be not offered too"
            }
        }

        Item{
            id: filler
            Layout.fillHeight: true
            Layout.columnSpan: 2
        }
        RowLayout{
            Layout.fillWidth: true
            Layout.columnSpan: 2
            Button{
                text: "Add Section"
                Layout.margins: 5
                Layout.fillWidth: true
                onClicked: angebote.model.addSection("New section",true)
            }
            Button{
                text: "Add Angebot"
                Layout.margins: 5
                Layout.fillWidth: true
                enabled: angebote.model.count > 0
                onClicked: angebote.model.addEntryAtIndex(angebote.currentIndex)
            }
        }

    }

}
