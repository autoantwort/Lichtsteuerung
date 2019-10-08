import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.3
import custom.licht 1.0
import "components"

ModelView{
    id: deviceModelView
    model: deviceModel
    nameFunction: function(modelData){return modelData.name + ", Channel: " + modelData.startDMXChannel + (modelData.prototype.numberOfChannels > 1 ? " - " + (modelData.startDMXChannel + modelData.prototype.numberOfChannels - 1) : "");}
    rows: 7
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
        Layout.row: 3
        Layout.column: 2
        text:"DevicePrototype:"
        Layout.rightMargin: 20
    }
    Text{
        Layout.row: 3
        Layout.column: 3
        text: parent.currentModelData ? parent.currentModelData.prototype.name : ""
        font.pixelSize: 15
        color: Material.secondaryTextColor
        TextUnderline{
            extendetWidth:1
            color:"lightgrey"
        }
    }
    Label{
        Layout.row: 4
        Layout.column: 2
        text:"Position:"
        Layout.rightMargin: 20
    }
    RowLayout{
        Layout.row: 4
        Layout.column: 3
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

    Dialog{
        modality: Qt.WindowModal
        id:dialog
        title: "Hallo"
        width:300
        contentItem: RowLayout {
            Pane{
                Layout.fillWidth: true
                ColumnLayout{
                    anchors.left: parent.left
                    anchors.right: parent.right
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
                }
            }
        }
    }

}
