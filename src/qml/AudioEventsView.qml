import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import custom.licht 1.0
import "components"

Item {
    property alias visibleForUser: dataView.visibleForUser;
    AudioEventDataView{
        id: dataView
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.right: sidebar.left
        Slider{
            anchors.horizontalCenter: parent.horizontalCenter;
            anchors.top: parent.top;
            anchors.topMargin: 20;
            width: 200
            from: 20
            to: 400
            value: 100
            stepSize: 1;
            onValueChanged: parent.pixelPerSecond = value;
        }
        Column{
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            width: 200
            Repeater{
                model: dataView.names;
                id: nameRepeater
                delegate: Label{
                    width: 200
                    height: dataView.height / nameRepeater.count
                    verticalAlignment: "AlignTop"
                    topPadding: Math.min(15, Math.max(0, height / 2 - 20))
                    leftPadding: 5
                    text: modelData;
                } // delegate
            } // Repeater
        } // Column
    }
    Item{
        property bool show: true
        width: show * 200;
        Behavior on width {
            NumberAnimation{
                duration: 200;
                easing.type: "OutCubic"
            }
        }
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        id: sidebar;

        RoundButton{
            anchors.top: parent.top
            anchors.right: parent.left
            id:roundButton
            text: sidebar.show ? "-" : "+";
            font.pixelSize: 20;
            onClicked: sidebar.show = !sidebar.show;

        }

        ScrollView{
            anchors.fill: parent
            anchors.right: undefined
            anchors.leftMargin: 5
            clip: true

            ScrollBar.vertical.policy: sidebar.show ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
            // scroll to the bottom at start
            Component.onCompleted: ScrollBar.vertical.position = 1 - ScrollBar.vertical.size;
            Column{
                Repeater{
                    id: rootRepeater
                    property var names: ["Beat Events", "Onset Events", "Onset Values"]
                    model: dataView.getNumberOfOnsetDetectionFunctions();
                    delegate: ColumnLayout{
                        width: 195
                        height: implicitHeight
                        property int onsetIndex: index
                        Label{
                            Layout.topMargin: 5
                            text: dataView.getNameOfOnsetDetectionFunctions(index);
                        }
                        Repeater{
                            model: 3
                            delegate: CheckBox{
                                Layout.preferredHeight: implicitHeight-16
                                Layout.fillWidth: true
                                text: rootRepeater.names[index]
                                onToggled: dataView.enableDetectionFor(onsetIndex, index, checked);
                                checked: dataView.isDetectionEnabledFor(onsetIndex, index);

                                Rectangle{
                                    id: colorPlane
                                    anchors.top: parent.top
                                    anchors.bottom: parent.bottom
                                    anchors.right: parent.right
                                    anchors.margins: 5
                                    anchors.rightMargin: 14
                                    visible: parent.checked
                                    width: height
                                    color: dataView.getColor(onsetIndex, index);
                                    onColorChanged: dataView.setColor(onsetIndex, index, color);
                                    radius: 3
                                    MouseArea{
                                        anchors.fill: parent
                                        onClicked: {
                                            colorDialog.component = parent
                                            colorDialog.startColor = parent.color;
                                            colorDialog.visible = true;
                                        }
                                    }
                                } // Rectangle
                            } // delegate
                        } // Repeater
                    } // delegate
                } // Repeater
            } // Column
        } // ScrollView

        ColorDialog{
            property var component
            id: colorDialog
            onCurrentColorChanged: component.color = currentColor
            onColorSelected: component = selectedColor;
        }
    } // sidebar
}
