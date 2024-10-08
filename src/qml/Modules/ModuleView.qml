import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import custom.licht 1.0
import QtQuick.Controls.Material
import QtQml
import QtQuick.Window
import "../components"

Item{
    id: root
    property bool visibleForUser: SwipeView.isCurrentItem

    Shortcut{
        enabled: visibleForUser
        sequences: ["Ctrl+B", "Ctrl+R"]
        autoRepeat: false
        onActivated: codeEditorHelper.compile();
    }

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
                property var currentModelData : currentItem ? currentItem.itemData : null
                id:listView
                delegate: ItemDelegate{
                    property var itemData : modelData
                    width: parent.width
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
                model: SortedModelVectorView{
                    id: sortedView
                    sourceModel: modulesModel
                }
                headerPositioning: ListView.OverlayHeader
                Component{
                    id: header
                    Pane {
                        Component.onCompleted: {
                            background.radius = 0;
                        }

                        width: listView.width
                        padding: 6
                        z: 2
                        Material.elevation: 4
                        RowLayout {
                            anchors.fill: parent
                            spacing: 6
                            Label{
                                text: "Sort by:"
                            }
                            ComboBox{
                                id: sortCommboBox
                                Layout.fillWidth: true
                                textRole: "name"
                                model: ListModel{
                                    ListElement{
                                        name: "Creation Date"
                                        sortPropertyName: ""
                                    }
                                    ListElement{
                                        name: "Name"
                                        sortPropertyName: "name"
                                    }
                                    ListElement{
                                        name: "Type"
                                        sortPropertyName: "type"
                                    }
                                    ListElement{
                                        name: "Spotify Responder"
                                        sortPropertyName: "spotifyResponder"
                                    }
                                    ListElement{
                                        name: "Input Type"
                                        sortPropertyName: "inputType"
                                    }
                                    ListElement{
                                        name: "Output Type"
                                        sortPropertyName: "outputType"
                                    }
                                }
                                property bool _firstTime: true
                                onCurrentIndexChanged: {
                                    if(_firstTime){ // if we dont do that, we geht a weird bug that said we cant load a component (testet 5.12.3)
                                        _firstTime = false;
                                        return;
                                    }
                                    listView.model.sortPropertyName = model.get(currentIndex).sortPropertyName;
                                }
                            }
                            Button{
                                icon.source: sortedView.sortOrder === Qt.DescendingOrder ? "/icons/sort_order/sort-reverse-alphabetical-order.svg" : "/icons/sort_order/sort-by-alphabet.svg"
                                onClicked: sortedView.sortOrder = sortedView.sortOrder === Qt.DescendingOrder ? Qt.AscendingOrder : Qt.DescendingOrder;
                                Layout.preferredWidth: 40
                            }
                        }
                    }
                }

                header: header
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
            enabled: listView.currentModelData
            text: listView.currentModelData ? listView.currentModelData.name : "Select one Module"
            onTextChanged: if(listView.currentModelData)listView.currentModelData.name = text;
            validator: RegularExpressionValidator{
               regularExpression: /[^\s]+/
            }
        }

        Label{
            text: "Description:"
        }
        TextInputField{
            Layout.fillWidth: true
            enabled: listView.currentModelData
            text: listView.currentModelData ? listView.currentModelData.description : "Select one Module"
            onTextChanged: if(listView.currentModelData)listView.currentModelData.description = text;
        }

        Label{
            text: "Type:"
        }
        RowLayout{
            ComboBox{
                model: moduleTypeModel
                Layout.preferredWidth: implicitWidth+5
                currentIndex: listView.currentModelData ? listView.currentModelData.type : 0;
                enabled: listView.currentModelData
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
                currentIndex: listView.currentModelData ? listView.currentModelData.inputType : 0;
                enabled: listView.currentModelData
                Layout.preferredWidth: implicitWidth+5
                onCurrentIndexChanged: if(listView.currentItem)listView.currentModelData.inputType = currentIndex;
            }
            Label{text: "Output:"
                visible: outputType.visible
            }
            ComboBox{
                id:outputType
                model: valueTypeList
                currentIndex: listView.currentModelData ? listView.currentModelData.outputType : 0
                enabled: listView.currentModelData;
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
            enabled: listView.currentModelData
            Layout.preferredHeight: model ? Math.max(50,Math.min(model.rowCount() * 20,120)) : 50
            /*onModelChanged:{ for (var prop in model) {
                                print(prop += " (" + typeof(model[prop]) + ") = " + model[prop]);
                            }
            }*/
            model: listView.currentModelData ? listView.currentModelData.properties : null
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
                    flat: true
                    width: height
                    onClicked: listView.currentModelData.removeProperty(delegate.modelEntry)
                    icon.source: "/icons/remove.svg"
                    icon.width: 24
                    icon.height: 24
                    padding: 1
                }
                Button{
                    id: addProp
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    anchors.topMargin: -4
                    anchors.bottomMargin: -4
                    width: height
                    flat: true
                    onClicked: {
                        dialog.prop = listView.currentModelData.createNewProperty();
                        dialog.visible = true;
                    }
                    icon.source: "/icons/add.svg"
                    icon.width: 24
                    icon.height: 24
                    padding: 1
                }
            }
            Button{
                visible: propertyView.count === 0
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                width: height
                flat: true
                onClicked: {
                    dialog.prop = listView.currentModelData.createNewProperty();
                    dialog.visible = true;
                }
                icon.source: "/icons/add.svg"
                icon.width: 24
                icon.height: 24
                padding: 1
            }
        }
        Label{
            text: "Spotify responder:"
        }
        CheckBox{
            checked: listView.currentModelData ? listView.currentModelData.spotifyResponder : false;
            enabled: listView.currentModelData
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
            onHoveredChanged: if(!hovered && listView.currentModelData)listView.currentModelData.code = codeEditor.text
            clip: true
            TextArea{
                z: 2
                property real lineHeight: contentHeight/lineCount
                font.family: "Courier New"
                font.pointSize: 12
                tabStopDistance: 16
                hoverEnabled: false

                id: codeEditor
                selectByMouse: true
                text: listView.currentModelData ? listView.currentModelData.code : "No Module selected"
                enabled: listView.currentModelData
                onCursorPositionChanged: {
                    if(codeCompletionPopup.visible){
                        codeEditorHelper.updateCodeCompletionModel(codeEditor.cursorPosition);
                    }
                }
                cursorDelegate: Rectangle{
                    width: 2;
                    color: Qt.rgba(.2,.2,.2,1);
                }

                //onTextChanged: listView.currentModelData.code = text
                Keys.onTabPressed: {
                    insert(cursorPosition,"  ");
                }
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
                            text: codeCompletionListView.currentModelData ? codeCompletionListView.currentModelData.description : "Keine Beschreibung"
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
                        property var currentModelData: currentItem ? currentItem.itemData : null;
                        delegate: ItemDelegate {
                            property var itemData: modelData
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
                        // deplay code and run at next iteration
                        timer.setTimeout(function(){
                            codeEditor.insert(codeEditor.cursorPosition,newText);
                            codeEditor.cursorPosition = pos;
                        }, 0);

                    }
                    onInformation:{
                        if (text.indexOf("error")>=0){
                            informationDialog.title = "Error while compiling";
                            informationDialog.text = text;
                        }else{
                            informationDialog.title = text;
                            informationDialog.text = "";
                        }
                        informationDialog.visible = true;
                    }
                }

                Timer {
                    // from https://stackoverflow.com/a/50224584/10162645
                    id: timer
                    function setTimeout(cb, delayTime) {
                        timer.interval = delayTime;
                        timer.repeat = false;
                        timer.triggered.connect(cb);
                        timer.triggered.connect(function release () {
                            timer.triggered.disconnect(cb); // This is important
                            timer.triggered.disconnect(release); // This is important as well
                        });
                        timer.start();
                    }
                }
            } // TextArea
            // Must be behind TextArea because of https://bugreports.qt.io/browse/QTBUG-62292
            Item{
                anchors.fill: codeEditor
                anchors.topMargin: codeEditor.topPadding
                TextMetrics{
                    font: codeEditor.font
                    text: "M"
                    id: textMetrics
                }

                Repeater{
                    model: codeEditorHelper.codeMarkups
                    Rectangle{
                        x: modelData.column * (textMetrics.advanceWidth)
                        y: modelData.row * height
                        width: modelData.markupLength * (textMetrics.advanceWidth)
                        height: codeEditor.lineHeight
                        color: modelData.error ? "red" : "orange"
                        MouseArea{
                            anchors.fill: parent
                            id: mouseArea
                            acceptedButtons: Qt.NoButton
                            hoverEnabled: true
                        }
                        ToolTip.text: modelData.message
                        ToolTip.visible: mouseArea.containsMouse
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

    Dialog{
        id: informationDialog
        modal: true
        standardButtons: Dialog.Ok
        Overlay.modal: ModalPopupBackground{}
        width: 600
        margins: 50
        leftPadding: header.leftPadding
        rightPadding: footer.rightPadding
        bottomPadding: 0
        x: (Overlay.overlay.width-width)/2;
        y: (root.height-height)/2;
        property alias text: dialogText.text
        onAboutToShow: {
            contentItem.ScrollBar.vertical.position = 0;
            contentItem.ScrollBar.horizontal.position = 0;
        }
        contentItem: ScrollView{
            clip: true;
            ScrollBar.vertical.policy: ScrollBar.vertical.visualSize===1 ? ScrollBar.AlwaysOff : ScrollBar.AlwaysOn
            Text{
                font.family: "Courier New"
                font.bold: true
                onContentWidthChanged: console.log("contentWidth", contentWidth)
                id: dialogText
                color: Material.foreground
            }
        }
    }

    Dialog{
        property var prop;
        modal: true
        closePolicy: Popup.CloseOnEscape

        Overlay.modal: ModalPopupBackground{}
        width: 300
        margins: 50
        padding: 10
        x: (root.width-width)/2;

        id: dialog
        header: Label{
            topPadding: 15
            text: "Add/Change Property"
            font.pointSize: 14
            font.bold: true
            verticalAlignment: "AlignVCenter"
            horizontalAlignment: "AlignHCenter"
        }
        contentItem: GridLayout{
            columns: 2
            Label{
                text: "Name"
            }
            TextInputField{
                id:name
                Layout.fillWidth: true
                text: dialog.prop ? dialog.prop.name : "Error"
                validator: RegularExpressionValidator{
                   regularExpression: /[a-z][a-z_0-9]*$/i
                }
            }
            Label{
                text:"Beschreibung"
            }
            TextInputField{
                id:besch
                Layout.fillWidth: true
                text: dialog.prop ? dialog.prop.description : "Error"
            }
            Label{
                text:"Typ"
            }
            ComboBox{
                currentIndex: dialog.prop ? dialog.prop.type : -1
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
                text: type.currentIndex === 4 ? "0" : type.currentIndex === 5 ? "" : (dialog.prop ? dialog.prop.minValue : "0")
            }

            Label{
                text:"max value"
            }
            TextInputField{
                Layout.fillWidth: true
                id:maxVal
                validator: IntValidator{}
                text: type.currentIndex === 4 ? "1" : type.currentIndex === 5 ? "" : (dialog.prop ? dialog.prop.maxValue : "")
            }
            Label{
                text:"default value"

            }
            TextInputField{
                text: type.currentIndex !== 5 && dialog.prop ? dialog.prop.defaultValue : ""
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
        } // contentItem: GridLayout
    } // Dialog
}
