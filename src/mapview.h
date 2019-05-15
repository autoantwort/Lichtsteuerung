#ifndef MAPVIEW_H
#define MAPVIEW_H

#include "polygon.h"
#include "gridbackground.h"
#include "programms/modulemanager.h"

namespace GUI{

class MapView : public GridBackground
{
    Q_OBJECT
    Q_PROPERTY(QPointF controlPoint READ getControlPoint WRITE setControlPoint NOTIFY controlPointChanged)
    QPointF controlPoint;
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
    void setControlPoint(const QPointF & p){
        if(p != controlPoint){
            controlPoint = p;
            emit controlPointChanged();
            Modules::ModuleManager::singletone()->controller().updateControlPoint(controlPoint);
        }
    }
    QPointF getControlPoint()const{return controlPoint;}
protected:
    void mousePressEvent(QMouseEvent *event)override;
    void mouseMoveEvent(QMouseEvent *event)override;
signals:
    void controlPointChanged();
public slots:
};

}

#endif // MAPVIEW_H
