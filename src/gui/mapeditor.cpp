#include "mapeditor.h"

namespace GUI {

MapEditor::MapEditor() {
    setAcceptedMouseButtons(Qt::AllButtons);
    setKeepMouseGrab(true);
}

void MapEditor::mousePressEvent(QMouseEvent *event){
    for(auto i = stonework->points.begin();i!=stonework->points.end();++i){
        if (i->isNear(event->position().x(), event->position().y(), 20)) {
            currentPressedPoint = &*i;
            event->accept();
        }
    }
}

void MapEditor::mouseMoveEvent(QMouseEvent *event){
    if(currentPressedPoint){
        currentPressedPoint->x = event->position().x();
        currentPressedPoint->y = event->position().y();
        stonework->update();
        event->accept();
    }
}

} // namespace GUI
