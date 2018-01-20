#ifndef MAPVIEW_H
#define MAPVIEW_H

#include "polygon.h"
#include "gridbackground.h"

class MapView : public GridBackground
{
    Q_OBJECT
protected:
    Polygon * stonework = new Polygon();
    Polygon * surfaces = new Polygon();
public:
    MapView();

signals:

public slots:
};

#endif // MAPVIEW_H
