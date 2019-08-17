import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.12

Popup {
    id: popup
    width: 200
    closePolicy: Popup.NoAutoClose
    property var entries: []
    property int currentIndex: -1
    // this property is used to avoid a binding loop in the onCurrentIndexChanged function
    property int lastCurrentIndex_: -1
    // this property holds the last current index
    property int lastCurrentIndex: -1
    property alias tooltipText: helpButton.tooltipText
    property alias helpButton: helpButton
    property bool hasPreviousEntry: false
    property bool hasNextEntry: false
    property bool allowEntryJumping: true
    property bool enableAnimations: true
    property HelpEntry currentEntry: null
    property int defaultPosition: HelpEntry.Position.East
    property point currentPosition: Qt.point(0,0);
    signal start;
    signal end;
    onAboutToHide: end();

    Behavior on x {
        NumberAnimation {
            duration: 500
            easing.type: Easing.OutCubic
        }
        enabled: enableAnimations && lastCurrentIndex >= 0 && lastCurrentIndex <= entries.length
    }
    Behavior on y {
        NumberAnimation {
            duration: 500
            easing.type: Easing.OutCubic
        }
        enabled: enableAnimations && lastCurrentIndex >= 0 && lastCurrentIndex <= entries.length
    }
    Behavior on implicitHeight {
        NumberAnimation {
            duration: 500
            easing.type: Easing.OutCubic
        }
        enabled: enableAnimations;
    }

    onOpened: contentItem.forceActiveFocus(Qt.OtherFocusReason)
    onCurrentIndexChanged: {
        if(lastCurrentIndex_ === currentIndex){
            currentEntry = currentIndex >= 0 && currentIndex < entries.length ? entries[currentIndex] : null;
            return;
        }
        // check if we probaly open the popup
        if(popup.visible === false && currentIndex >= 0 && currentIndex < entries.length)
            start();
        // call leave method of old entry
        if(currentEntry!==null)
            currentEntry.leave();
        // compute next index if currentIndex is not visible
        const dir = lastCurrentIndex_ < currentIndex ? 1 : -1;
        let i = currentIndex;
        while(i >= 0 && i < entries.length && !entries[i].visible){
            i += dir;
        }
        lastCurrentIndex = lastCurrentIndex_;
        lastCurrentIndex_ = i;
        currentIndex = i;
        // determine if we have next/previous entry and compute the page counter
        let hasPreviousEntry_ = false;
        let hasNextEntry_ = false;
        let pageCount = currentIndex >= 0 && currentIndex < entries.length ? 1 : 0;
        for(let n = i + 1;n < entries.length;n++){
            if(entries[n].visible){
                hasNextEntry_ = true;
                ++pageCount;
            }
        }
        let realCurrentIndex = 0;
        for(let k = i - 1;k >= 0;k--){
            if(entries[k].visible){
                hasPreviousEntry_ = true;
                ++pageCount;
                ++realCurrentIndex;
            }
        }
        pageIndicator.count = pageCount;
        if(currentIndex >= 0 && currentIndex < entries.length)
            pageIndicator.currentIndex = realCurrentIndex;

        hasNextEntry = hasNextEntry_;
        hasPreviousEntry = hasPreviousEntry_;
        let newCurrentEntry = currentIndex >= 0 && currentIndex < entries.length ? entries[currentIndex] : null;

        currentEntry = newCurrentEntry;
        if(currentEntry !== null){
            currentPosition = currentEntry.component.mapToItem(popup.parent,0,0);
        }
    }
    Timer{
        interval: 15
        onTriggered: if(currentEntry)currentPosition = currentEntry.component.mapToItem(popup.parent,0,0);
        repeat: true
        running: popup.visible
    }

    function computeRealIndex(index){
        if(index < 0 || index >= entries.length)
            return 0;
        for(let i = 0; i <= index && i < entries.length;++i){
            if(!entries[i].visible){
                ++index;
            }
        }
        return index;
    }

    function setCurrentIndex(index){
        currentIndex = computeRealIndex(index);
    }

    padding: 10
    onCurrentEntryChanged: {
        if(currentEntry !== null){
            currentEntry.enter();
            titel.text = currentEntry.titel;
            explanation.text = currentEntry.explanation;
            popup.visible = true;
            switch(currentEntry.position){
            case HelpEntry.Position.East:
                arrow.state = "east";
                break;
            case HelpEntry.Position.West:
                arrow.state = "west";
                break;
            case HelpEntry.Position.North:
                arrow.state = "north";
                break;
            case HelpEntry.Position.South:
                arrow.state = "south";
                break;
            }
        }else{
            arrow.state = "";
            popup.visible = false;
        }

    }

    background: Pane{
        padding: 0
        Component.onCompleted: background.color = "white"
        Material.elevation: 24

        Rectangle{
            id: arrow
            color: "white"
            rotation: 45
            width: 20
            height: 20
            states: [
                State {
                    name: "east"
                    id: ifHorizontal
                    property real yPosition: currentEntry ? currentPosition.y + (currentEntry.component.height - page.implicitHeight)/2  + currentEntry.yShift : yPosition
                    AnchorChanges {
                        target: arrow
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.left
                    }
                    PropertyChanges {
                        target: arrow
                        anchors.verticalCenterOffset: Math.min(0, -3 + ifHorizontal.yPosition) - (Overlay.overlay ? Math.min(0,Overlay.overlay.height - (ifHorizontal.yPosition + page.implicitHeight + popup.padding*2 - 3)) : 0)
                    }
                    PropertyChanges {
                        target: popup
                        restoreEntryValues: false
                        x: currentEntry ? currentPosition.x + currentEntry.component.width + 20 + currentEntry.xShift : x
                        y: Math.min(Overlay.overlay.height - page.implicitHeight - popup.padding*2 - 3, Math.max(3, ifHorizontal.yPosition))
                    }
                },
                State {
                    name: "north"
                    AnchorChanges {
                        target: arrow
                        anchors.verticalCenter: parent.bottom
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                    PropertyChanges {
                        target: popup
                        restoreEntryValues: false
                        x: currentEntry ? currentPosition.x + (currentEntry.component.width - width)/2 + currentEntry.xShift : x
                        y: currentEntry ? currentPosition.y - page.implicitHeight - 20 + currentEntry.yShift : y
                    }
                },
                State {
                    name: "west"
                    AnchorChanges {
                        target: arrow
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.right
                    }
                    PropertyChanges {
                        target: arrow
                        anchors.verticalCenterOffset: Math.min(0, -3 + ifHorizontal.yPosition) - (Overlay.overlay ? Math.min(0,Overlay.overlay.height - (ifHorizontal.yPosition + page.implicitHeight + popup.padding*2 - 3)) : 0)
                    }
                    PropertyChanges {
                        target: popup
                        restoreEntryValues: false
                        x: currentEntry ? currentPosition.x - width - 20 + currentEntry.xShift : x
                        y: Math.min(Overlay.overlay.height - page.implicitHeight - popup.padding*2 - 3, Math.max(3, ifHorizontal.yPosition))
                    }
                },
                State {
                    name: "south"
                    AnchorChanges {
                        target: arrow
                        anchors.verticalCenter: parent.bottom
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                    PropertyChanges {
                        target: popup
                        restoreEntryValues: false
                        x: currentEntry ? currentPosition.x + (currentEntry.component.width - width)/2 + currentEntry.xShift : x
                        y: currentEntry ? currentPosition.y + currentEntry.component.height + 20 + currentEntry.yShift : y
                    }
                }
            ]
            transitions: [
                Transition {
                    AnchorAnimation{
                        targets: arrow
                        duration: 500
                        easing.type: Easing.OutCubic
                    }
                    NumberAnimation {
                        target: popup
                        properties: "x,y"
                        duration: 500
                        easing.type: Easing.OutCubic
                    }
                    enabled: enableAnimations
                },
                Transition {
                    from: ""
                    NumberAnimation {
                        target: popup
                        properties: "x,y"
                        duration: 0
                    }
                    enabled: enableAnimations
                }
            ]
        }
    }




    Component.onCompleted: {
        for(var i in contentData){
            if(contentData[i] instanceof HelpEntry){
                entries.push(contentData[i]);
            }
        }
    }


    contentItem: Page{
        id: page
        Component.onCompleted: background.color = "white"

        Keys.onRightPressed: if(hasNextEntry)++currentIndex;
        Keys.onLeftPressed: if(hasPreviousEntry)--currentIndex;
        focus: true


        header: Label{
            id: titel
            font.bold: true

            Text {
                text: "x"
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.topMargin: -3
                color: mouseArea.containsMouse ? "black" : "gray"
                MouseArea{
                    id: mouseArea
                    anchors.fill: parent
                    anchors.margins: -4
                    hoverEnabled: true
                    onClicked: popup.currentIndex = -1;
                }
            }
        }
        contentItem: Label{
            id: explanation
            wrapMode: "WordWrap"
            clip: true
        }
        footer: Item{
            implicitHeight: buttonRow.implicitHeight + 10
            RowLayout{
                id: buttonRow
                anchors.fill: parent
                Button{
                    text: "Previous"
                    Layout.fillWidth: true
                    Layout.preferredHeight: implicitHeight - 10
                    Layout.bottomMargin: -5
                    enabled: hasPreviousEntry;
                    onClicked: --currentIndex;
                }
                Button{
                    text: !hasNextEntry ? "Close" : "Next"
                    Layout.fillWidth: true
                    Layout.preferredHeight: implicitHeight - 10
                    Layout.bottomMargin: -5
                    onClicked: hasNextEntry ? ++currentIndex : currentIndex = -1
                }
            }
            PageIndicator{
                id: pageIndicator
                anchors.top: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.topMargin: -8
                interactive: popup.allowEntryJumping
                onCurrentIndexChanged: setCurrentIndex(currentIndex)
                delegate: Rectangle {
                    implicitWidth: 8
                    implicitHeight: 8
                    ToolTip.text: popup.entries[computeRealIndex(index)].titel
                    ToolTip.visible: hoverRect.containsMouse
                    ToolTip.delay: 700
                    MouseArea{
                        anchors.fill: parent
                        hoverEnabled: true
                        acceptedButtons: Qt.NoButton
                        id: hoverRect
                    }

                    radius: width / 2
                    color: pageIndicator.enabled ? pageIndicator.Material.foreground : pageIndicator.Material.hintTextColor

                    opacity: index === currentIndex ? 0.95 : pressed ? 0.7 : 0.45
                    Behavior on opacity { OpacityAnimator { duration: 100 } }
                }
            }
        }


    }
    HelpButton{
        id: helpButton
        visible: currentEntry === null
        parent: popup.parent
        onClicked: currentIndex = 0;
    }
}
