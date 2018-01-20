#ifndef GRIDBACKGROUND_H
#define GRIDBACKGROUND_H

#include <QQuickItem>

class GridBackground : public QQuickItem
{
    int distanceBetweenLines = 15;
    QColor backgroundColor = QColor(51,51,51);
    QColor lineColor = QColor(128,128,128);
public:
    GridBackground();
    void setLineColor(QColor c){lineColor=c;update();}
    void setBackgroundColor(QColor c){backgroundColor=c;update();}
    void setDistanceBetweenLines(int d){distanceBetweenLines=d;update();}
protected:
    virtual QSGNode * updatePaintNode(QSGNode *, UpdatePaintNodeData *)override;
};

#endif // GRIDBACKGROUND_H
