#include "mapeditor.h"

namespace GUI {

MapEditor::MapEditor() {
    setAcceptedMouseButtons(Qt::AllButtons);
    setKeepMouseGrab(true);
}

void MapEditor::mousePressEvent(QMouseEvent *event) {
    qDebug() << event->x() << '\n';
    for (auto i = stonework->points.begin(); i != stonework->points.end(); ++i) {
        if (i->isNear(event->x(), event->y(), 20)) {
            currentPressedPoint = &*i;
            event->accept();
        }
    }
}

void MapEditor::mouseMoveEvent(QMouseEvent *event) {
    if (currentPressedPoint) {
        currentPressedPoint->x = event->x();
        currentPressedPoint->y = event->y();
        stonework->update();
        event->accept();
    }
}

} // namespace GUI
