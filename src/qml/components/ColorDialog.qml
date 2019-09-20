import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12

Window {
    id: root
    width: 300
    height: 300
    flags: Qt.Window | Qt.WindowStaysOnTopHint | Qt.CustomizeWindowHint |
           Qt.WindowSystemMenuHint | Qt.WindowTitleHint | Qt.WindowCloseButtonHint
    property color currentColor;
    property color startColor;
    onVisibleChanged: if(visible)swipeView.updateColorsAtCurrentPane();
    signal colorSelected(color selectedColor);
    onClosing: colorSelected(currentColor);

    function showDialog(startColor){
        currentColor = startColor;
        this.startColor = startColor;
        visible = true;
    }

    ColumnLayout{
        anchors.fill: parent
        TabBar{
            id: tabBar
            Layout.fillWidth: true
            currentIndex: swipeView.currentIndex;
            TabButton{
                text: "RGB"
            }
            TabButton{
                text: "HSB"
            }
        }
        SwipeView{
            id: swipeView
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: tabBar.currentIndex;
            interactive: false
            onCurrentIndexChanged: updateColorsAtCurrentPane();
            function updateColorsAtCurrentPane(){
                if(swipeView.currentIndex === 0){
                    red.value = root.currentColor.r;
                    green.value = root.currentColor.g;
                    blue.value = root.currentColor.b;
                }else{
                    hue.value = Math.max(0,root.currentColor.hsvHue);
                    saturation.value = root.currentColor.hsvSaturation;
                    brightness.value = root.currentColor.hsvValue;
                }
            }

            Pane{
                ColumnLayout{
                    anchors.fill: parent
                    property var selectiorColor: currentColor.hslLightness > 0.3 ? "black" : "white"
                    Label{
                        text: "Red"
                    }
                    ColorSlider{
                        Layout.fillWidth: true
                        Layout.preferredHeight: 20
                        id: red
                        selectorColor: parent.selectiorColor
                        onValueChanged:{
                            root.currentColor.r = value;
                        }
                        gradient: Gradient{
                            GradientStop{ position: 0; color: Qt.rgba(0, green.value, blue.value, 1);}
                            GradientStop{ position: 1; color: Qt.rgba(1, green.value, blue.value, 1);}
                        }
                    }

                    Label{
                        text: "Green"
                    }
                    ColorSlider{
                        Layout.fillWidth: true
                        Layout.preferredHeight: 20
                        id: green
                        value: 1
                        selectorColor: parent.selectiorColor
                        onValueChanged: {
                            root.currentColor.g = value;
                        }
                        gradient: Gradient{
                            GradientStop{ position: 0; color: Qt.rgba(red.value, 0, blue.value, 1);}
                            GradientStop{ position: 1; color: Qt.rgba(red.value, 1, blue.value, 1);}
                        }
                    }

                    Label{
                        text: "Blue"
                    }
                    ColorSlider{
                        Layout.fillWidth: true
                        Layout.preferredHeight: 20
                        id: blue
                        value: 1
                        selectorColor: parent.selectiorColor
                        onValueChanged:{
                            root.currentColor.b = value;
                        }
                        gradient: Gradient{
                            GradientStop{ position: 0; color: Qt.rgba(red.value, green.value, 0, 1);}
                            GradientStop{ position: 1; color: Qt.rgba(red.value, green.value, 1, 1);}
                        }
                        Component.onCompleted: {
                            console.log("color rgb",Qt.rgba(1,0,0,1))
                            console.log("color hsl",Qt.hsla(1,0,0,1))
                            let c = Qt.hsla(.5,1,.5,1);
                            console.log("Farbe:")
                            for(let i in c){
                                console.log(i, " : ", c[i]);
                            }
                        }
                    }
                }
            }
            Pane{
                ColumnLayout{
                    anchors.fill: parent
                    // if the brightness is hight and the color is not blue or the saturation in not hight, then we choose black, otherwise white
                    property var selectiorColor: brightness.value > 0.65 && (hue.value < 200/360 || hue.value > 280/360 || saturation.value < 0.6) ? "black" : "white"
                    Label{
                        text: "Hue"
                    }
                    ColorSlider{
                        Layout.fillWidth: true
                        Layout.preferredHeight: 20
                        id: hue
                        selectorColor: parent.selectiorColor
                        onValueChanged:{
                            root.currentColor.hsvHue = value;
                        }
                        gradient: Gradient{
                            GradientStop{ position:   0/360; color: Qt.hsva(  0/360,saturation.value,brightness.value,1);}
                            GradientStop{ position:  60/360; color: Qt.hsva( 60/360,saturation.value,brightness.value,1);}
                            GradientStop{ position: 120/360; color: Qt.hsva(120/360,saturation.value,brightness.value,1);}
                            GradientStop{ position: 180/360; color: Qt.hsva(180/360,saturation.value,brightness.value,1);}
                            GradientStop{ position: 240/360; color: Qt.hsva(240/360,saturation.value,brightness.value,1);}
                            GradientStop{ position: 300/360; color: Qt.hsva(300/360,saturation.value,brightness.value,1);}
                            GradientStop{ position: 360/360; color: Qt.hsva(360/360,saturation.value,brightness.value,1);}
                        }
                    }

                    Label{
                        text: "Saturation"
                    }
                    ColorSlider{
                        Layout.fillWidth: true
                        Layout.preferredHeight: 20
                        id: saturation
                        value: 1
                        selectorColor: parent.selectiorColor
                        onValueChanged:{
                            root.currentColor.hsvSaturation = value;
                        }
                        gradient: Gradient{
                            GradientStop{ position: 0; color: Qt.hsva(hue.value,0,brightness.value,1);}
                            GradientStop{ position: 1; color: Qt.hsva(hue.value,1,brightness.value,1);}
                        }
                    }

                    Label{
                        text: "Brightness"
                    }
                    ColorSlider{
                        Layout.fillWidth: true
                        Layout.preferredHeight: 20
                        id: brightness
                        value: 1
                        selectorColor: parent.selectiorColor
                        onValueChanged:{
                            root.currentColor.hsvValue = value;
                        }
                        gradient: Gradient{
                            GradientStop{ position: 0; color: Qt.hsva(hue.value,saturation.value,0,1);}
                            GradientStop{ position: 1; color: Qt.hsva(hue.value,saturation.value,1,1);}
                        }
                        Component.onCompleted: {
                            console.log("color rgb",Qt.rgba(1,0,0,1))
                            console.log("color hsl",Qt.hsla(1,0,0,1))
                            let c = Qt.hsla(.5,1,.5,1);
                            console.log("Farbe:")
                            for(let i in c){
                                console.log(i, " : ", c[i]);
                            }
                        }
                    }
                }
            }
        }
        Item{
            Layout.fillWidth: true
            Layout.preferredHeight: buttonOk.implicitHeight + buttonOk.anchors.margins * 2
            Button{
                id: buttonAbort
                text: "Farbe zurücksetzen"
                anchors.margins: 5
                anchors.top: parent.top
                anchors.right: buttonOk.left
                anchors.bottom: parent.bottom
                onClicked: {
                    root.currentColor = startColor;
                    root.visible = false;
                }
            }
            Button{
                id: buttonOk
                text: "Schlißen"
                anchors.margins: 5
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                onClicked: root.visible = false;
            }
        }
    }
}
