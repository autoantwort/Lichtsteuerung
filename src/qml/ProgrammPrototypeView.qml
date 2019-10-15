import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import custom.licht 1.0
import "components"

ModelView{
    model: programmPrototypeModel
    id: modelView

    addButton.text: "Add Prototype"
    removeButton.text: "Remove Prototype"
    onAddClicked: dialog.visible = true
    onRemoveClicked: ModelManager.removeDmxProgramPrototype(remove);
    rows: 5
    sortModel: ListModel{
        ListElement{
            name: "Creation Date"
            sortPropertyName: ""
        }
        ListElement{
            name: "Name"
            sortPropertyName: "name"
        }
        ListElement{
            name: "Device Prototype"
            sortPropertyName: "devicePrototype.name"
        }
    }
    Label{
        Layout.topMargin: 4
        Layout.columnSpan: 2
        text: "Die einzelnen ChannelProgramme :"
        font.underline: true
    }

    ListView{
        clip: true
        Layout.columnSpan: 2
        Layout.fillHeight: true
        Layout.fillWidth: true
        id: channelView
        spacing: 32
        model: modelView.currentModelData ? modelView.currentModelData.channelProgramms : null
        ScrollBar.vertical: ScrollBar {
            policy: channelView.contentHeight > channelView.height ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
        }
        delegate: ColumnLayout{
            width: channelView.width
            RowLayout{
                Layout.preferredHeight: 50
                Layout.fillWidth: true
                spacing: 8
                Label{
                    text: editor.channelProgramm.channel.name
                    font.bold: true
                    Layout.minimumWidth: 60
                }
                Label{
                    text: "Time: "
                    visible: editor.currentTimePoint.hasCurrent
                }
                TextInputField{
                    Layout.minimumWidth: 30
                    text: editor.currentTimePoint.time.toFixed(2)
                    visible: editor.currentTimePoint.hasCurrent
                    onAccepted: editor.currentTimePoint.time = text
                    validator: DoubleValidator{}
                }
                Label{
                    text:"Value: "
                    visible: editor.currentTimePoint.hasCurrent
                }
                TextInputField{
                    Layout.minimumWidth: 30
                    text: editor.currentTimePoint.value
                    visible: editor.currentTimePoint.hasCurrent
                    onAccepted: editor.currentTimePoint.value = text
                    validator: IntValidator{bottom: 0; top:255}
                }
                Label{
                    text: "Curve type:"
                    visible: editor.selectedEasingCurve >= 0
                }
                ComboBox{
                    model: easingModel
                    property bool open: false
                    Layout.preferredWidth: 150
                    currentIndex: editor.selectedEasingCurve
                    visible: editor.selectedEasingCurve >= 0
                    onDownChanged: {
                        open|=down;
                    }
                    onHighlighted: {
                        if(open)editor.selectedEasingCurve = index
                    }
                    onActivated: {
                        editor.selectedEasingCurve = index
                        open=false;
                    }
                }
            } // RowLayout
            ChannelProgrammEditor{
                id: editor
                Layout.preferredHeight: 100
                Layout.fillWidth: true
                clickRadius: 36
                channelProgramm: modelData
                Rectangle{
                    anchors.fill: parent
                    color: "lightgrey"
                    z: -1
                }
                Rectangle{
                    x: editor.hoverSegmentX
                    z: -0.5
                    width: editor.hoverSegmentLength
                    height: 100
                    color: Qt.rgba(.7,.7,.7)
                }
                Rectangle{
                    x: editor.selectedSegmentX
                    z: -0.4
                    width: editor.selectedSegmentLength
                    height: 100
                    color: Qt.rgba(.5,.5,.5)
                }                
                Rectangle{
                    anchors.top: parent.bottom
                    visible: editor.mouseX >= 0
                    x: editor.mouseX
                    width: 1
                    height: 4
                    color: Material.foreground
                    Label{
                        anchors.top: parent.bottom
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.horizontalCenterOffset: editor.mouseX < contentWidth/2 ? -(editor.mouseX - contentWidth/2) : 0
                        horizontalAlignment: Text.AlignHCenter
                        text: editor.getValueForVisualX(editor.mouseX) + " at\n" + editor.getTimeForVisualX(editor.mouseX).toFixed(2);
                    }
                }
            } // ChannelProgrammEditor
        } // delegate: ColumnLayout
    } // ListView

    Label{
        Layout.columnSpan: 2
        Layout.fillWidth: true
        Layout.margins: 5
        wrapMode: Label.WrapAtWordBoundaryOrAnywhere
        text: "Hold N and click to create a new TimePoint. You can select a TimePoint to change its values. A selected TimePoint can be deleted with d. You can drag a TimePoint. Hold x while scrolling to change the zooming."
    }

    Popup{
        modal: true
        id: dialog
        width: 300
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        contentItem: ColumnLayout{
            spacing: 10
            ComboBox{
                Layout.fillWidth: true
                id: prototype
                model: devicePrototypeModel
                textRole: "display"
            }
            RowLayout{
                Label{
                    id: nameLabel
                    text: "Name :"
                }
                TextInputField{
                    Layout.fillWidth: true
                    id: name
                }
            }
            RowLayout{
                Label{
                    text: "Description :"
                }
                TextInputField{
                    Layout.fillWidth: true
                    id: description
                }
            }
            RowLayout{
                Button{
                    Layout.fillWidth: true
                    text: "Abbrechen"
                    onClicked: dialog.visible = false
                }
                Button{
                    Layout.fillWidth: true
                    text: "Erzeugen"
                    onClicked: {
                        if(name.text===""){
                            name.underlineColor = "red";
                        }else{
                            dialog.visible = false;
                            ModelManager.addProgrammPrototype(prototype.currentIndex,name.text,description.text);
                        }
                    }
                } // Button
            } // RowLayout
        } // contentItem: ColumnLayout
    } // Popup
}
