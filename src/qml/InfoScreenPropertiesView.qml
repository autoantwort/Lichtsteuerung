import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.12
import custom.licht 1.0
import "InformationScreen"

Item{
    property var window: InformationWindow{}
    property AngebotsModel angebote: AngebotsModel{}
    GridLayout{
        columns: 2
        anchors.fill: parent
        anchors.leftMargin: 5
        Label{
            text: "Modality:"
        }
        RowLayout{
            Layout.fillWidth: true
            RadioButton{
                text: "Hide"
                onCheckedChanged: if(checked) window.visibility = Window.Hidden;
                checked: window.visibility === Window.Hidden || window.visibility === Window.Minimized
            }
            RadioButton{
                text: "Windowed"
                onCheckedChanged: if(checked) window.visibility = Window.Windowed;
                checked: window.visibility === Window.Maximized || window.visibility === Window.Windowed
            }
            RadioButton{
                text: "Full Screen"
                onCheckedChanged: if(checked) window.visibility = Window.FullScreen;
                checked: window.visibility === Window.FullScreen
            }
        }
        Label{
            text: "Background color:"
        }
        Button{
            id:colorButton
            text: "Select Color"
            Component.onCompleted: {
                colorButton.background.color = window.color;
                colorDialog.color = window.color;
            }

            ColorDialog{
                id: colorDialog
                onRejected: {
                    colorButton.background.color = color;
                    window.color = color;
                }
                onCurrentColorChanged: {
                    colorButton.background.color = currentColor;
                    window.color = currentColor;
                }
            }
            onClicked: colorDialog.open();
        }
        AngebotsManagmentComponent{
            Layout.columnSpan: 2
            model: angebote
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}
