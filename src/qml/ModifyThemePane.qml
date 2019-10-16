import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import custom.licht 1.0
import "components"

ColumnLayout {
    id: root
    RowLayout{
        Label{
            Layout.leftMargin: 10
            Layout.fillWidth: true
            text: "Theme:"
        }
        ComboBox{
            model: ["Light", "Dark"]
            currentIndex: Settings.theme
            onCurrentIndexChanged: Settings.theme = currentIndex
        }
        Button{
            text: "Reset this theme"
            Layout.rightMargin: 10
            onClicked: {
                Settings.accentColor = undefined;
                Settings.accentMaterial = undefined;
                Settings.accentShade = undefined;
                Settings.foregroundColor = undefined;
                Settings.foregroundMaterial = undefined;
                Settings.foregroundShade = undefined;
                Settings.backgroundColor = undefined;
                Settings.backgroundMaterial = undefined;
                Settings.backgroundShade = undefined;
                Settings.popupBackgroundEffect = undefined;
                Settings.popupBackgroundEffectIntensity = undefined;
            }
        }
    }
    RowLayout{
        Label{
            Layout.leftMargin: 10
            Layout.fillWidth: true
            text: "Popup background:"
        }
        ComboBox{
            Layout.preferredWidth: 90
            model: ["Dim", "Blur"]
            currentIndex: Settings.popupBackgroundEffect
            onCurrentIndexChanged: Settings.popupBackgroundEffect = currentIndex
        }
        ComboBox{
            Layout.preferredWidth: 110
            Layout.rightMargin: 10
            model: ["Weak", "Normal", "Strong"]
            currentIndex: Settings.popupBackgroundEffectIntensity
            onCurrentIndexChanged: Settings.popupBackgroundEffectIntensity = currentIndex
        }
    }


    TabBar{
        id: target
        Layout.fillWidth: true
        position: TabBar.Footer
        TabButton{
            text: "Accent"
        }
        TabButton{
            text: "Foreground"
        }
        TabButton{
            text: "Background"
        }
    }
    function updateColor(){
        let color;
        if(!colorSelection.isMaterialColor){
            color = colorSelection.getColor(colorSelection.currentIndex);
        }else{
            if(shade.currentIndex === 0){
                color = Material.color(colorSelection.materialIndex);
            }else{
                color = Material.color(colorSelection.materialIndex, shade.currentIndex - 1);
            }
        }
        switch(target.currentIndex){
        case 0: Settings.accentColor = color; break;
        case 1: Settings.foregroundColor = color; break;
        case 2: Settings.backgroundColor = color; break;
        }
    }
    RowLayout{
        ListView{
            property color customColor;
            onCustomColorChanged: {
                switch(target.currentIndex){
                case 0: Settings.accentColor = customColor; break;
                case 1: Settings.foregroundColor = customColor; break;
                case 2: Settings.backgroundColor = customColor; break;
                }
            }

            readonly property int numOwn: 1
            property bool isMaterialColor: currentIndex >= numOwn
            property int materialIndex: currentIndex - numOwn
            function getColor(index){
                switch(index){
                case 0: return customColor;
                default: return Material.color(index - numOwn);
                }
            }
            function updateCurrentIndex(){
                switch(target.currentIndex){
                case 0:
                    customColor = Settings.accentColor;
                    currentIndex = Settings.accentMaterial >= 0 ? Settings.accentMaterial + numOwn : 0;
                    return;
                case 1:
                    customColor = Settings.foregroundColor;
                    currentIndex = Settings.foregroundMaterial >= 0 ? Settings.foregroundMaterial + numOwn : 0;
                    return;
                case 2:
                    customColor = Settings.backgroundColor;
                    currentIndex = Settings.backgroundMaterial >= 0 ? Settings.backgroundMaterial + numOwn : 0;
                    return;
                }
                console.error("Error in ModifyThemePane at customColor.currentIndex: ...")
            }
            Component.onCompleted: {
                Settings.themeChanged.connect(updateCurrentIndex);
                Settings.accentMaterialChanged.connect(updateCurrentIndex);
                Settings.backgroundMaterialChanged.connect(updateCurrentIndex);
                Settings.foregroundMaterialChanged.connect(updateCurrentIndex);
                target.currentIndexChanged.connect(updateCurrentIndex);
                updateCurrentIndex();
            }

            keyNavigationEnabled: true
            Layout.fillHeight: true
            Layout.preferredWidth: root.width/2
            clip: true
            id: colorSelection
            model: ["Custom", "Red", "Pink", "Purple", "Deep Purple", "Indigo", "Blue", "Light Blue", "Cyan", "Teal", "Green", "Light Green", "Lime", "Yellow", "Amber", "Orange", "Deep Orange", "Brown", "Grey", "Blue Grey"]
            delegate: RadioDelegate{
                width: colorSelection.width
                background: Rectangle{
                    color: colorSelection.getColor(index)
                }
                text: modelData;
                checked: colorSelection.currentIndex === index;
                onClicked: {
                    colorSelection.currentIndex = index;
                    switch(target.currentIndex){
                    case 0: Settings.accentMaterial = index - colorSelection.numOwn; break;
                    case 1: Settings.foregroundMaterial = index - colorSelection.numOwn; break;
                    case 2: Settings.backgroundMaterial = index - colorSelection.numOwn; break;
                    }
                    root.updateColor();
                }
            }
            maximumFlickVelocity: 600
            boundsBehavior: Flickable.DragOverBounds
        }
        Item{
            Layout.fillHeight: true
            Layout.preferredWidth: root.width/2

            RowLayout{
                visible: !colorSelection.isMaterialColor
                anchors.fill: parent
                anchors.rightMargin: 15
                ColorSlider{
                    Layout.fillHeight: true
                    Layout.preferredWidth: parent.width/3-2
                    orientation: Gradient.Vertical
                    value: colorSelection.customColor.hsvHue
                    onValueChanged: if(!colorSelection.isMaterialColor)colorSelection.customColor.hsvHue = value;
                    gradient: Gradient{
                        GradientStop{ position:   0/360; color: Qt.hsva(  0/360,colorSelection.customColor.hsvSaturation,colorSelection.customColor.hsvValue,1);}
                        GradientStop{ position:  60/360; color: Qt.hsva( 60/360,colorSelection.customColor.hsvSaturation,colorSelection.customColor.hsvValue,1);}
                        GradientStop{ position: 120/360; color: Qt.hsva(120/360,colorSelection.customColor.hsvSaturation,colorSelection.customColor.hsvValue,1);}
                        GradientStop{ position: 180/360; color: Qt.hsva(180/360,colorSelection.customColor.hsvSaturation,colorSelection.customColor.hsvValue,1);}
                        GradientStop{ position: 240/360; color: Qt.hsva(240/360,colorSelection.customColor.hsvSaturation,colorSelection.customColor.hsvValue,1);}
                        GradientStop{ position: 300/360; color: Qt.hsva(300/360,colorSelection.customColor.hsvSaturation,colorSelection.customColor.hsvValue,1);}
                        GradientStop{ position: 360/360; color: Qt.hsva(360/360,colorSelection.customColor.hsvSaturation,colorSelection.customColor.hsvValue,1);}
                    }
                }

                ColorSlider{
                    Layout.fillHeight: true
                    Layout.preferredWidth: parent.width/3-2
                    orientation: Gradient.Vertical
                    value: colorSelection.customColor.hsvSaturation
                    onValueChanged: if(!colorSelection.isMaterialColor)colorSelection.customColor.hsvSaturation = value;
                    gradient: Gradient{
                        GradientStop{ position: 0; color: Qt.hsva(colorSelection.customColor.hsvHue,0,colorSelection.customColor.hsvValue,1);}
                        GradientStop{ position: 1; color: Qt.hsva(colorSelection.customColor.hsvHue,1,colorSelection.customColor.hsvValue,1);}
                    }
                }
                ColorSlider{
                    Layout.fillHeight: true
                    Layout.preferredWidth: parent.width/3-2
                    orientation: Gradient.Vertical
                    value: colorSelection.customColor.hsvValue
                    onValueChanged: if(!colorSelection.isMaterialColor)colorSelection.customColor.hsvValue = value;
                    gradient: Gradient{
                        GradientStop{ position: 0; color: Qt.hsva(colorSelection.customColor.hsvHue,colorSelection.customColor.hsvSaturation,0,1);}
                        GradientStop{ position: 1; color: Qt.hsva(colorSelection.customColor.hsvHue,colorSelection.customColor.hsvSaturation,1,1);}
                    }
                }
            }

            ListView{
                visible: colorSelection.isMaterialColor
                anchors.fill: parent
                enabled: colorSelection.currentIndex !== 0
                keyNavigationEnabled: true
                function updateCurrentIndex(){
                    switch(target.currentIndex){
                    case 0: currentIndex = Settings.accentShade >= 0 ? Settings.accentShade + 1 : 0; return;
                    case 1: currentIndex = Settings.foregroundShade >= 0 ? Settings.foregroundShade + 1 : 0; return;
                    case 2: currentIndex = Settings.backgroundShade >= 0 ? Settings.backgroundShade + 1 : 0; return;
                    }
                    console.error("Error in ModifyThemePane at customColor.currentIndex: ...")
                }
                Component.onCompleted: {
                    Settings.themeChanged.connect(updateCurrentIndex);
                    Settings.accentShadeChanged.connect(updateCurrentIndex);
                    Settings.backgroundShadeChanged.connect(updateCurrentIndex);
                    Settings.foregroundShadeChanged.connect(updateCurrentIndex);
                    target.currentIndexChanged.connect(updateCurrentIndex);
                }

                clip: true
                id: shade
                model: ["Default", "Shade 50", "Shade 100", "Shade 200", "Shade 300", "Shade 400", "Shade 500", "Shade 600", "Shade 700", "Shade 800", "Shade 900", "Shade A100", "Shade A200", "Shade A400", "Shade A700"]
                delegate: RadioDelegate{
                    width: shade.width
                    background: Rectangle{
                        color: colorSelection.currentIndex === 0 && root.Material.theme>=0 ? "white" : index === 0 ? Material.color(colorSelection.materialIndex) : Material.color(colorSelection.materialIndex, index - 1);
                    }
                    text: modelData;
                    checked: shade.currentIndex === index;
                    onClicked: {
                        shade.currentIndex = index;
                        switch(target.currentIndex){
                        case 0: Settings.accentShade = index - colorSelection.numOwn; break;
                        case 1: Settings.foregroundShade = index - colorSelection.numOwn; break;
                        case 2: Settings.backgroundShade = index - colorSelection.numOwn; break;
                        }
                        root.updateColor();
                    }
                }
                maximumFlickVelocity: 600
                boundsBehavior: Flickable.DragOverBounds
            }
        }
    }
}
