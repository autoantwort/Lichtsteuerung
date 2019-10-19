import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtGraphicalEffects 1.0

Window {
    color: "black"
    visibility: SlideShow.windowVisibility
    property int _currentState;
    property int _oldState;
    property bool showFirst: true;
    onVisibilityChanged: {
        _oldState = _currentState;
        _currentState = visibility;
        SlideShow.windowVisibility = visibility;
    }
    Component.onCompleted: {
        SlideShow.onLoadNextImage.connect(path => {
            if(path[0] === '/'){
                path = path.substring(1);
            }
            if (showFirst) {
                secondBackground.source = "file:///" + path;
                secondImage.source = "file:///" + path;
            } else {
                firstBackground.source = "file:///" + path;
                firstImage.source = "file:///" + path;
            }
        });
        SlideShow.showNextImage.connect(() => {
            showFirst = !showFirst;
            if (showFirst){
                secondAni.start();
            } else {
                firstAni.start();
            }
        });
        screen = Qt.application.screens[Qt.application.screens.length - 1];
    }

    Shortcut{
        sequence: StandardKey.Cancel
        onActivated: if(visibility === Window.FullScreen) SlideShow.windowVisibility = Window.Maximized;
    }

    Item {
        anchors.fill: parent
        id: background

        Image {
            anchors.fill: parent
            id: firstBackground
            asynchronous: true
            onStatusChanged: {
                if(status === Image.Error){
                    SlideShow.reportInvalidImage();
                }
            }
        }
        Image {
            anchors.fill: parent
            id: secondBackground
            opacity: !showFirst
            asynchronous: true
            onStatusChanged: {
                if(status === Image.Error){
                    SlideShow.reportInvalidImage();
                }
            }
            Behavior on opacity {
                NumberAnimation{
                    duration: 1000
                }
            }
        }
    }
    FastBlur{
        anchors.fill: parent
        radius: 128
        source: background
    }
    RectangularGlow {
        anchors.margins: 20
        anchors.fill: firstImage
        glowRadius: 60
        spread: 0.2
        color: "black"
        opacity: firstImage.opacity
        z: 1
    }
    Image {
        height: Math.min(parent.height * (2/3),sourceSize.height)
        anchors.centerIn: parent
        id: firstImage
        opacity: showFirst
        fillMode: Image.PreserveAspectFit
        z: 1
        asynchronous: true
        Behavior on opacity {
            NumberAnimation{
                duration: 500
            }
        }
        NumberAnimation {
            id: firstAni
            targets: [firstImage, firstBackground]
            property: "scale"
            easing.type: Easing.InOutQuad
            duration: 2000
            from: 1
            to : 1.5
            onFinished: firstBackground.scale = firstImage.scale = 1
        }
    }
    RectangularGlow {
        anchors.margins: 20
        anchors.fill: secondImage
        glowRadius: 60
        spread: 0.2
        color: "black"
        opacity: secondImage.opacity
        z: showFirst ? 2 : .5
    }
    Image {
        fillMode: Image.PreserveAspectFit
        height: Math.min(parent.height * (2/3),sourceSize.height)
        anchors.centerIn: parent
        id: secondImage
        opacity: 1 - firstImage.opacity
        z: showFirst ? 2 : .5
        asynchronous: true
        NumberAnimation {
            id: secondAni
            targets: [secondImage, secondBackground]
            easing.type: Easing.InOutQuad
            property: "scale"
            duration: 2000
            from: 1
            to : 1.5
            onFinished: secondBackground.scale = secondImage.scale = 1
        }
    }
    MouseArea{
        anchors.fill: parent
        hoverEnabled: true
        opacity: containsMouse
        Behavior on opacity { NumberAnimation{ easing.type: Easing.OutCubic; } }
        RoundButton{
            text: SlideShow.windowVisibility === Window.FullScreen ? "Exit Fullscreen" : "Enter Fullscreen"
            onClicked: {
                if (SlideShow.windowVisibility === Window.FullScreen){
                    SlideShow.windowVisibility = _oldState;
                } else {
                    SlideShow.windowVisibility = Window.FullScreen;
                }
            }
        }
    }
    Text{
        anchors.centerIn: parent
        font.pointSize: 30
        text: "No images"
        color: "white"
        visible: !SlideShow.hasImages
    }
}
