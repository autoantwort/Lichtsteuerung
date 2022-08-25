#ifndef MAPVIEW_H
#define MAPVIEW_H

#include "gridbackground.h"
#include "polygon.h"
#include "modules/modulemanager.h"

namespace GUI {

class MapView : public GridBackground {
    Q_OBJECT
    Q_PROPERTY(QPointF controlPoint READ getControlPoint WRITE setControlPoint NOTIFY controlPointChanged)
    Q_PROPERTY(QPointF translation READ getTranslation NOTIFY translationChanged)
    QPointF translation;
    QPointF controlPoint;
    static MapView *lastCreated;

protected:
    std::vector<GUI::Polygon *> polygons;
    GUI::Polygon *stonework;
    GUI::Polygon *surfaces;

public:
    MapView();
    static MapView *getLastCreated() { return lastCreated; }
    void loadFromJsonObject(const QJsonObject &o);
    void writeJsonObject(QJsonObject &o) const;
    void setControlPoint(QPointF p) {
        p.setX(p.x() - translation.x() - 3);
        p.setY(p.y() - translation.y() - 3);
        if (p.x() < 0) {
            p.setX(0);
        } else if (p.x() > implicitWidth() - translation.x()) {
            p.setX(implicitWidth() - translation.x());
        }
        if (p.y() < 0) {
            p.setY(0);
        } else if (p.y() > implicitHeight() - translation.y()) {
            p.setY(implicitHeight() - translation.y());
        }
        if (p != controlPoint) {
            controlPoint = p;
            emit controlPointChanged();
            Modules::ModuleManager::singletone()->controller().updateControlPoint(controlPoint);
        }
    }
    QPointF getControlPoint() const { return controlPoint; }
    QPointF getTranslation() const { return translation; }

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
signals:
    void controlPointChanged();
    void translationChanged();
public slots:
};

} // namespace GUI

#endif // MAPVIEW_H
