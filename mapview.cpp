#include "mapview.h"

MapView::MapView()
{
    stonework = new Polygon();
    stonework->setParentItem(this);

    surfaces = new Polygon();
    surfaces->setParentItem(this);
    surfaces->setColor(QColor(255, 153, 0));
    surfaces->addRectangle(200,250,10,40);

    stonework->addPoint(Polygon::Point(1.f,2.f));
    stonework->addPoint(Polygon::Point(50.f,7.f));
    stonework->addPoint(Polygon::Point(4.f,80.f));
    stonework->addPoint(Polygon::Point(80.f,100.f));

    stonework->addTriangle(0,1,2);
    stonework->addTriangle(1,2,3);
    stonework->setColor(QColor(0,128,255));
    //stonework->addRectangle(100,200,200,300);

    stonework->addArc(200,170,200,200,230,200);
    stonework->addArc(230,200,200,200,200,230);
    //stonework->addArc(70,200,100,200,100,170);
}
