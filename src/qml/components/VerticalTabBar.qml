import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Controls.Material 2.3

TabBar{
    id:control
    contentItem: ListView{        
        model: control.contentModel
        currentIndex: control.currentIndex        

        spacing: control.spacing
        orientation: ListView.Vertical
        boundsBehavior: Flickable.StopAtBounds
        flickableDirection: Flickable.AutoFlickIfNeeded

        highlightMoveDuration: 250
        highlightResizeDuration: 0
        highlightFollowsCurrentItem: true

        highlight: Item{
            Rectangle{
                anchors.left: parent.left;
                anchors.top: parent.top;
                anchors.bottom: parent.bottom;
                width: 3
                color: Material.accent
            }
        }
    }
    background: Rectangle {
        color: Qt.darker(control.Material.backgroundColor,1.02)
    }
}
