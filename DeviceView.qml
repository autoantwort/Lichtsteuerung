import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

Item{
    height: 100
    width: parent.width
    Text {
        text: devs.model.rowCount()
    }
    ListView{
        id:devs
        model: deviceModel
        anchors.fill: parent
        delegate: ItemDelegate{
            text:model.modelData
        }
    }
}
