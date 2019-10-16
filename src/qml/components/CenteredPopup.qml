import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12

Popup{
    modal: true
    Overlay.modal: ModalPopupBackground{}
    // if we use center in, a developer dont has the ability to change the positon of the Popup
    // anchors.centerIn: Overlay.overlay
    parent: Overlay.overlay
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
}
