import QtQuick 2.11
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import custom.licht 1.0
import QtQuick.Controls.Material 2.2
import QtQuick.Dialogs 1.2
import "components"

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
                property var currentModelData : currentItem.itemData
                id:listView
                delegate: ItemDelegate{
                    property var itemData : modelData
                    width: parent.width
                    height: 60
                    text: modelData.name +  " (" + moduleTypeModel[modelData.type] + ')'
                    onClicked: {
                        listView.currentIndex = index;
                    }
                    MouseArea{
                        acceptedButtons: "RightButton"
                        anchors.fill: parent
                        onClicked: {
                            var pos = listView.mapFromItem(parent,mouse.x, mouse.y);
                            duplicateMenu.x = pos.x;
                            duplicateMenu.y = pos.y;
                            duplicateMenu.visible = true
                            duplicateMenu.index = index;
                        }
                    }

                }
                highlight: Rectangle{
                    color: "blue"
                    opacity: 0.7
                }
                model: modulesModel
                Menu{
                    id:duplicateMenu
                    property int index;
                    MenuItem{
                        text: "Duplicate"
                        onClicked: {
                            ModelManager.duplicateModule(duplicateMenu.index);
                            listView.currentIndex = listView.count-1;
                        }
                    }

                }
            }
            footer: RowLayout{
                Button{
                    Layout.preferredWidth: listView.width/2-5
                    Layout.leftMargin: 5
                    id: buttonAdd
                    text:"Add"
                    font.pixelSize: 15
                    onClicked: {
                        ModelManager.addModule();
                        listView.currentIndex = listView.count - 1;
                    }
                }
                Button{
                    Layout.preferredWidth: listView.width/2-10
                    Layout.rightMargin: 10
                    id: buttonRemove
                    text:"Remove"
                    font.pixelSize: 15
                    onClicked: aksWhenRemoveDialog.visible = true
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
            text: listView.currentItem ? listView.currentModelData.name : "Select one Module"
            onTextChanged: if(listView.currentItem)listView.currentModelData.name = text;
            validator: RegExpValidator{
                regExp: /[^\s]+/
            }
        }

        Label{
            text: "Description:"
        }
        TextInputField{
            Layout.fillWidth: true
            enabled: listView.currentIndex !== -1
            text: listView.currentItem ? listView.currentModelData.description : "Select one Module"
            onTextChanged: if(listView.currentItem)listView.currentModelData.description = text;
        }

        Label{
            text: "Type:"
        }
        RowLayout{
            ComboBox{
                model: moduleTypeModel
                Layout.preferredWidth: implicitWidth+5
                currentIndex: listView.currentModelData.type
                onCurrentIndexChanged: {
                    if(listView.currentItem)listView.currentModelData.type = currentIndex;
                    if(currentIndex == 0/*Program*/ || currentIndex == 1/*LoopProgram*/){
                        outputType.visible = true;
                        inputType.visible = false;
                    }else if(currentIndex == 2/*Filter*/){
                        outputType.visible = true;
                        inputType.visible = true;
                    }
                }
            }
            Label{
                text: "Input:"
                visible:inputType.visible
            }
            ComboBox{
                id: inputType
                model: valueTypeList
                currentIndex: listView.currentModelData.inputType
                Layout.preferredWidth: implicitWidth+5
                onCurrentIndexChanged: if(listView.currentItem)listView.currentModelData.inputType = currentIndex;
            }
            Label{text: "Output:"
                visible: outputType.visible
            }
            ComboBox{
                id:outputType
                model: valueTypeList
                currentIndex: listView.currentModelData.outputType
                Layout.preferredWidth: implicitWidth+5
                onCurrentIndexChanged: if(listView.currentItem)listView.currentModelData.outputType = currentIndex;
            }
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
            /*onModelChanged:{ for (var prop in model) {
                                print(prop += " (" + typeof(model[prop]) + ") = " + model[prop]);
                            }
            }*/
            model: listView.currentModelData.properties
            maximumFlickVelocity: 400
            delegate: ItemDelegate{
                id:delegate
                property var modelEntry : modelData
                text: modelData.name
                width: propertyView.width
                height: 20
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
                    onClicked: listView.currentModelData.removeProperty(delegate.modelEntry)
                    Image {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        height: 24
                        width: 24
                        source: "/icons/remove.svg"
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
                        dialog.prop = listView.currentModelData.createNewProperty();
                        dialog.visible = true;
                    }
                    Image {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        height: 24
                        width: 24
                        source: "/icons/add.svg"
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
                    dialog.prop = listView.currentModelData.createNewProperty();
                    dialog.visible = true;
                }
                Image {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    height: 24
                    width: 24
                    source: "/icons/add.svg"
                }
            }
        }
        Label{
            text: "Spotify responder:"
        }
        CheckBox{
            checked: listView.currentModelData.spotifyResponder;
            onCheckStateChanged: listView.currentModelData.spotifyResponder = checked;
            Layout.preferredHeight: 29
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
            onHoveredChanged: if(!hovered)listView.currentModelData.code = codeEditor.text
            TextArea{
                font.family: "Liberation Mono"
                font.pointSize: 10
                tabStopDistance: 16
                id: codeEditor
                selectByMouse: true
                text: listView.currentModelData.code
                onCursorPositionChanged: {
                    if(codeCompletionPopup.visible){
                        codeEditorHelper.updateCodeCompletionModel(codeEditor.cursorPosition);
                    }
                }
                //onTextChanged: listView.currentModelData.code = text
                Keys.onDownPressed: {
                    if(codeCompletionPopup.visible){
                        event.accepted = true;
                        codeCompletionListView.incrementCurrentIndex();
                    }else{
                        event.accepted = false;
                    }
                }
                Keys.onUpPressed: {
                    if(codeCompletionPopup.visible){
                        event.accepted = true;
                        codeCompletionListView.decrementCurrentIndex();
                    }else{
                        event.accepted = false;
                    }
                }

                Keys.onSpacePressed: {
                    if(event.modifiers & (Qt.MetaModifier|Qt.ControlModifier)){
                        event.accepted = true;
                        var old = codeCompletionPopup.visible;
                        //descriptionPopup.visible = !old;
                        codeCompletionPopup.visible = !old;

                    }else{
                        event.accepted = false;
                    }
                }                
                Keys.onReturnPressed: {
                    if(codeCompletionPopup.visible){
                        event.accepted = true;
                        codeCompletionListView.clickCurrentItem();
                    }else{
                        event.accepted = false;
                    }
                }

                Popup{
                    id: descriptionPopup
                    x: codeCompletionPopup.x
                    y: codeCompletionPopup.y - 20 - descriptionPopup.height
                    width: codeCompletionPopup.width
                    padding: 0
                    height: descriptionLabel.implicitHeight + 4*2
                    contentItem: Rectangle{
                        Label{
                            anchors.margins: 4
                            anchors.fill: parent
                            id: descriptionLabel
                            wrapMode: "WordWrap"
                            text: codeCompletionListView.currentItem ? odeCompletionListView.currentModelData.description : "Keine Beschreibung"
                            background: null
                        }
                        color: "beige"
                    }

                }

                Popup{
                    padding: 0
                    id:codeCompletionPopup                    
                    x: codeEditor.cursorRectangle.x
                    y: codeEditor.cursorRectangle.y + codeEditor.cursorRectangle.height
                    width: contentItem.implicitWidth
                    height: Math.max(Math.min(150,codeCompletionListView.count*20),20)
                    onAboutToShow: {
                        codeEditorHelper.updateCodeCompletionModel(codeEditor.cursorPosition);
                        descriptionPopup.visible = codeCompletionListView.count > 0;
                        codeCompletionListView.updateWidth();
                    }
                    onAboutToHide: descriptionPopup.visible = false
                    contentItem: ListView{
                        clip: true
                        id: codeCompletionListView
                        model: codeEditorHelper.codeCompletions                        
                        delegate: ItemDelegate {
                            property var modelData: modelData
                            text: modelData.completion.replace(/\n*\t*/g,"")
                            onClicked: codeCompletionListView.clickCurrentItem()
                            highlighted: ListView.isCurrentItem
                            width: codeCompletionPopup.width
                            height: 20
                            leftPadding: 1
                            rightPadding: 1
                            onHoveredChanged: {
                                if(hovered){
                                    codeCompletionListView.currentIndex = index;
                                }
                            }
                            background: Rectangle{
                                color: parent.ListView.isCurrentItem?Qt.darker("beige"):"beige"
                            }
                        }
                        implicitWidth: 150
                        onCountChanged: {
                            if(codeCompletionPopup.opened){
                                descriptionPopup.visible = count > 0;
                            }
                            updateWidth();
                        }
                        function updateWidth(){
                            var max = 150;
                            for(var child in codeCompletionListView.contentItem.children){
                                max = Math.max(max,codeCompletionListView.contentItem.children[child].implicitWidth);
                            }
                            codeCompletionListView.implicitWidth = max;
                        }
                        function clickCurrentItem(){
                            if(codeCompletionListView.currentIndex!==-1){
                                console.log("close after " + codeCompletionListView.currentModelData.closeAfterCompletion);
                                if(codeCompletionListView.currentModelData.closeAfterCompletion){
                                    codeCompletionPopup.visible = false;
                                    descriptionPopup.visible = false;
                                }
                                // finde heraus was schon eingegeben wurde
                                var start = codeEditor.cursorPosition-1;
                                while(start>=0 && /[\w_\d]/.test(codeEditor.text.charAt(start))){
                                    --start;
                                }
                                // wenn wir uns in dem zu vervollständigem Wort befinden und das Wort schon vervollständigt im code steht, springen wir mit dem Cursor zum Ende des Wortes
                                var completion = codeCompletionListView.currentModelData.completion;
                                if(codeEditor.text.substr(start+1,completion.length) === codeCompletionListView.completion){
                                    codeEditor.cursorPosition += completion.length - (codeEditor.cursorPosition-start) + 1;
                                    return;
                                }
                                codeEditor.insert(codeEditor.cursorPosition,completion.substring(codeEditor.cursorPosition-start-1));
                            }
                        }
                    }                    
                }

                CodeEditorHelper{
                    id:codeEditorHelper
                    module: listView.currentModelData
                    document: codeEditor.textDocument
                    onInsertText: {
                        console.log(newText);
                        codeEditor.insert(codeEditor.cursorPosition,newText);
                        // Hack to display all new text, sometimes new text disappear
                        //codeEditor.selectAll();
                        //codeEditor.deselect();
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


    AskWhenRemovePopup{
        id: aksWhenRemoveDialog
        onYesClicked:{
            var index = listView.currentIndex;
            if(index === 0){
                listView.currentIndex = 1;
            }else{
                listView.currentIndex = index - 1;
            }
            ModelManager.removeModule(index);
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
                        top:  Number(maxVal.text)
                        bottom: Number(minVal.text)
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
