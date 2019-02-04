#ifndef MAPEDITOR_H
#define MAPEDITOR_H

#include "mapview.h"

namespace GUI{

class MapEditor : public MapView
{
private:
    GUI::Polygon::Point * currentPressedPoint = nullptr;
public:
    MapEditor();

protected:
    virtual void mouseMoveEvent(QMouseEvent *event)override;
    virtual void mousePressEvent(QMouseEvent *event)override;
};

}

#endif // MAPEDITOR_H
