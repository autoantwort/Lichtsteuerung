#ifndef MAPVIEW_H
#define MAPVIEW_H

#include "polygon.h"
#include "gridbackground.h"

class MapView : public GridBackground
{
    Q_OBJECT
    static MapView * lastCreated;
protected:
    std::vector<Polygon *> polygons;
    Polygon * stonework;
    Polygon * surfaces;
public:
    MapView();
    static MapView * getLastCreated(){return lastCreated;}
    void loadFromJsonObject(const QJsonObject &o);
    void writeJsonObject(QJsonObject &o)const;
signals:

public slots:
};

#endif // MAPVIEW_H
