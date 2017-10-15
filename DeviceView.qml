import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0


    ListView{
        width: 400
        id:devs
        model: deviceModel
        anchors.fill: parent
        delegate: ItemDelegate{
            text: name
        }
    }

