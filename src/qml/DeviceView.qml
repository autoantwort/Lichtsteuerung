import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import custom.licht 1.0
import "components"

ModelView{
    id: deviceModelView
    model: deviceModel
    nameFunction: function(modelData){return modelData.name + ", Channel: " + modelData.startDMXChannel + (modelData.prototype.numberOfChannels > 1 ? " - " + (modelData.startDMXChannel + modelData.prototype.numberOfChannels - 1) : "");}
    rows: 8
    onAddClicked: dialog.visible = true
    onRemoveClicked: ModelManager.removeDmxDevice(remove);
    addButtonEnabled: UserManagment.currentUser.havePermission(Permission.ADD_DEVICE)
    removeButtonEnabled: UserManagment.currentUser.havePermission(Permission.REMOVE_DEVICE)
    nameInputEnabled: UserManagment.currentUser.havePermission(Permission.CHANGE_NAME)
    descriptionInputEnabled: UserManagment.currentUser.havePermission(Permission.CHANGE_NAME)
    addButton.text: "Add Device"
    removeButton.text: "Remove Device"
    property var placeOnMapCallback
    sortModel: ListModel{
        ListElement{
            name: "Creation Date"
            sortPropertyName: ""
        }
        ListElement{
            name: "DMX Channel"
            sortPropertyName: "startDMXChannel"
        }
        ListElement{
            name: "Name"
            sortPropertyName: "name"
        }
        ListElement{
            name: "Prototype Name"
            sortPropertyName: "prototype.name"
        }
    }
    searchHelpText: "You can search by text and by dmx channel:\nUse '(>|>=|<|<=|=) x' or 'x - y' to search for a range"
    property int minDisplayedChannel: 0
    property int maxDisplayedChannel: 100000
    property bool checkChannels: false
    // to allow e.g. 'Scanner > 80'
    property string additionalSearchString
    onCurrentSearchTextChanged: {
        checkChannels = false
        additionalSearchString = "";
        const string = currentSearchText.replace(/ +/g, '');
        // detect not completed >, >=, ...
        if(/[<>=]{1,2}[a-zA-Z ]*$/.exec(string)!==null){
            checkChannels = true;
        }
        // find additional string, like 'Scanner >= 80'
        const str = /[a-zA-Z ]+/.exec(string);
        if(str !== null){
            console.log(str)
            additionalSearchString = str[0];
        }
        // search for ranges: x-y
        const range = /(\d+)-(\d+)/.exec(string);
        if(range !== null){
            checkChannels = true;
            minDisplayedChannel = Math.min(range[1], range[2]);
            maxDisplayedChannel = Math.max(range[1], range[2]);
            return
        }
        minDisplayedChannel = 0;
        maxDisplayedChannel = 10000;
        const gt = />(=)?(\d+)/.exec(string);
        if(gt !== null){
            checkChannels = true;
            minDisplayedChannel = Number(gt[2]) + (gt[1] === undefined ? 1 : 0);
        }
        const lt = /<(=)?(\d+)/.exec(string);
        if(lt !== null){
            checkChannels = true;
            maxDisplayedChannel = Number(lt[2]) - (lt[1] === undefined ? 1 : 0);
        }
        if(!checkChannels){
            const eq = /=(\d+)/.exec(string);
            if(eq !== null){
                checkChannels = true;
                minDisplayedChannel = maxDisplayedChannel = eq[1];
            }
        }
        if(!checkChannels){
            additionalSearchString = currentSearchText.trim();
        }
    }
    searchFilter: (device, text) => {
        if(checkChannels){
            return device.startDMXChannel + device.prototype.numberOfChannels > minDisplayedChannel && device.startDMXChannel <= maxDisplayedChannel && text.indexOf(additionalSearchString) !== -1
        }else{
            return text.indexOf(additionalSearchString) !== -1;
        }
    }

    Label{
        Layout.row: 2
        Layout.column: 2
        text:"First DMX-Channel:"
        Layout.rightMargin: 20
    }
    TextInputField{
        Layout.row: 2
        Layout.column: 3
        enabled: UserManagment.currentUser.havePermission(Permission.CHANGE_DEVICE_DMX_CHANNEL);
        text: parent.currentModelData ? parent.currentModelData.startDMXChannel : ""
        validator: IntValidator{
            bottom: 0
            top:512
        }
        onTextChanged: if(parent.currentModelData) parent.currentModelData.startDMXChannel = text.length?text:0
    }
    Label{
        text: "Pin configuration:"
    }
    RowLayout{
        Layout.preferredHeight: 30
        Layout.fillWidth: true
        Layout.margins: 3
        spacing: 0
        id: pinComp
        property int currentDMXChannel: deviceModelView.currentModelData ? deviceModelView.currentModelData.startDMXChannel : 0
        onCurrentDMXChannelChanged: {
            let v = currentDMXChannel;
            for(let i = 8; i >= 0; --i){
                const expo = Math.pow(2,i);
                visibleChildren[i + 1].on = v >= expo;
                if(v >= expo){
                    v -= expo;
                }
            }
        }
        function updateStartChannel(){
            let v = 0;
            for(let i = 0; i < 9; ++i){
                if(visibleChildren[i + 1].on){
                    v += Math.pow(2,i);
                }
            }
            deviceModelView.currentModelData.startDMXChannel = v;
        }

        Repeater{
            model: 10
            Item{
                Layout.alignment: Qt.AlignTop
                Layout.preferredWidth: 12
                property bool on: true
                Rectangle{
                    id: switchRect
                    width: 10
                    height: 22
                    border.color: Material.iconDisabledColor
                    border.width: 1
                    color: "transparent"
                    Rectangle{
                        width: parent.width - 4
                        height: parent.height/2 - 2
                        x: 2
                        y: 2 + (1 - parent.parent.on) * (parent.height/2 - 2)
                        color: Material.foreground
                        Behavior on y{
                            NumberAnimation{easing.type: Easing.OutQuint; duration: 100}
                        }
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            switchRect.parent.on = !switchRect.parent.on;
                            pinComp.updateStartChannel();
                        }
                        hoverEnabled: true
                        ToolTip.delay: 1000
                        ToolTip.visible: containsMouse
                        ToolTip.text: index === 0 ? "Referenz" : "2^" + (index - 1) + " = " + Math.pow(2, index - 1)
                    }
                }
                Text{
                    anchors.top: switchRect.bottom
                    anchors.topMargin: 3
                    anchors.horizontalCenter: switchRect.horizontalCenter
                    color: Material.foreground
                    text: index === 0 ? switchRect.parent.on ? "On" : "Off" : (index - 1)
                }
            }
        }
    }


    Label{
        text:"DevicePrototype:"
        Layout.rightMargin: 20
    }
    Text{
        text: parent.currentModelData ? parent.currentModelData.prototype.name : ""
        font.pixelSize: 15
        color: Material.secondaryTextColor
        TextUnderline{
            extendetWidth:1
            color:"lightgrey"
        }
    }
    Label{
        text:"Position:"
        Layout.rightMargin: 20
    }
    RowLayout{
        Text{
            text:"x:"
            color: Material.foreground
            font.pixelSize: 15
        }
        TextInputField{
            enabled: UserManagment.currentUser.havePermission(Permission.CHANGE_POSITION);
            Layout.minimumWidth: 40
            text:deviceModelView.currentModelData ? deviceModelView.currentModelData.position.x : ""
            validator: IntValidator{}
            onTextChanged: if(deviceModelView.currentModelData) deviceModelView.currentModelData.position.x = text.length?text:0
        }
        Text{
            Layout.leftMargin: 10
            text:"y:"
            color: Material.foreground
            font.pixelSize: 15
        }
        TextInputField{
            enabled: UserManagment.currentUser.havePermission(Permission.CHANGE_POSITION);
            Layout.minimumWidth: 40
            text: deviceModelView.currentModelData ? deviceModelView.currentModelData.position.y : ""
            validator: IntValidator{}
            onTextChanged: if(deviceModelView.currentModelData) deviceModelView.currentModelData.position.y = text.length?text:0
        }
        Button{
            enabled: UserManagment.currentUser.havePermission(Permission.CHANGE_POSITION);
            visible: deviceModelView.currentModelData ? deviceModelView.placeOnMapCallback && deviceModelView.currentModelData.position.x === -1 || deviceModelView.currentModelData.position.x === -1 : false
            text: "Place on map"
            onClicked: deviceModelView.placeOnMapCallback(deviceModelView.currentModelData)
            height: implicitHeight - 15
        }
    }

    Popup{
        modal: true
        id:dialog
        width:300
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        contentItem: ColumnLayout{
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
        } // contentItem: ColumnLayout
    } // Popup
}
