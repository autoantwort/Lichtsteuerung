import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtGraphicalEffects 1.0

Item {
    property int blockWidth: 1
    property int blockHeight: 1
    onBlockWidthChanged: width=blockWidth*50
    onBlockHeightChanged: height=blockHeight*50
    width: 50
    height: 50
}
