#ifndef MAPVIEW_H
#define MAPVIEW_H

#include "polygon.h"
#include "gridbackground.h"

namespace GUI{

class MapView : public GridBackground
{
    Q_OBJECT
    static MapView * lastCreated;
protected:
    std::vector<GUI::Polygon *> polygons;
    GUI::Polygon * stonework;
    GUI::Polygon * surfaces;
public:
    MapView();
    static MapView * getLastCreated(){return lastCreated;}
    void loadFromJsonObject(const QJsonObject &o);
    void writeJsonObject(QJsonObject &o)const;
signals:

public slots:
};

}

#endif // MAPVIEW_H
