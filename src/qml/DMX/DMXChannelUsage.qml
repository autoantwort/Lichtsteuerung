import QtQuick 2.12
import QtQuick.Controls 2.12

Item {
    id: root
    clip: true
    implicitHeight: 24 * 512 / 32
    implicitWidth: 24 * 32
    Repeater {
        model: 512
        delegate: Text {
            text: index
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            width: 25
            height: 25
            font.pixelSize: 10
            x: (index % 32) * 24
            y: Math.floor(index / 32) * 24
        }
    }
    Repeater {
        model: deviceModel
        delegate: Repeater {
            id: item
            property bool isDragged: false
            property var device: modelData
            property int realModel: (device.startDMXChannel + device.prototype.numberOfChannels) % 32 <= device.startDMXChannel % 32 ? 2 : 1
            model: isDragged ? model: realModel // if we change the model while dragging the delegate gets recreated and we loose mouse focus
            delegate: Rectangle {
                border.width: 1
                border.color: Qt.rgba(Math.random(), Math.random(), Math.random())
                color: Qt.darker(Settings.backgroundColor)
                y: (Math.floor(device.startDMXChannel / 32) + index) * 24
                height: 24
                implicitWidth: device.prototype.numberOfChannels * 24
                id: dragTarget
                Text {
                    id: text
                    anchors.fill: parent
                    text: item.isDragged ? device.startDMXChannel : mouse.containsMouse ? device.startDMXChannel + ": " + device.name : device.name
                    elide: Text.ElideRight
                    color: Settings.foregroundColor
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    width: mouse.containsMouse ? implicitWidth : parent.width
                }
                MouseArea {
                    id: mouse
                    anchors.fill: parent
                    hoverEnabled: true
                    acceptedButtons: index === 0 ? Qt.LeftButton : Qt.NoButton
                    onPositionChanged: {
                        if (mouse.buttons & Qt.LeftButton){
                            const p = root.mapFromItem(dragTarget, mouse.x, mouse.y);
                            if(p.x < 2 || p.y < 2 || p.x > root.implicitWidth - 5 || p.y > root.implicitHeight - 5){
                                return;
                            }
                            const x = Math.floor(p.x / 24);
                            const y = Math.floor(p.y / 24);
                            const dmx = y * 32 + x;
                            for(let i = 0; i < deviceModel.rowCount(); ++i){
                                const index = deviceModel.index(i, 0);
                                const d = deviceModel.data(index, 0x0100 + 1 /* ModelDataRole */);
                                if(device === d)
                                    continue;
                                if(dmx < d.startDMXChannel + d.prototype.numberOfChannels && dmx + device.prototype.numberOfChannels > d.startDMXChannel){
                                    return;
                                }
                            }
                            device.startDMXChannel = dmx;
                        }
                    }
                    onPressed: item.isDragged = true;
                    onReleased: item.isDragged = false
                }
                z: mouse.containsMouse ? 1000 : 1
                width: mouse.containsMouse && !item.isDragged ? text.implicitWidth > implicitWidth ? text.implicitWidth + 10 : implicitWidth : implicitWidth
                x: mouse.containsMouse && !item.isDragged ? index === 0 ? /* first row and hovered: */(device.startDMXChannel % 32 * 24 + width > 32 * 24 + 1 /* are we now to long with normal pos? */ ? 32 * 24 - width : device.startDMXChannel % 32 * 24) : /* second row and hovered: */ 0 : /* normal pos: */ (device.startDMXChannel % 32) * 24 - (index * 32 * 24 /* in next row */)
            }
        }
    }
}
