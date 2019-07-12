#ifndef COLORPLOT_H
#define COLORPLOT_H


#include <QQuickItem>
#include <list>
#include <mutex>

namespace GUI{

class Colorplot : public QQuickItem
{
    Q_OBJECT
    unsigned int blockSize = 512;
    QColor lineColor = QColor(0,0,0);
    Q_PROPERTY(QColor lineColor READ getLineColor WRITE setLineColor NOTIFY lineColorChanged)
    static Colorplot * lastCreated;
    const static unsigned int MAX_BLOCKS_COUNT = 900;


public:
    Colorplot();
    ~Colorplot(){
        if(lastCreated==this)
            lastCreated = nullptr;
    }
    static Colorplot * getLast(){return nullptr;}

    void setLineColor(QColor c){lineColor=c;update();lineColorChanged();}
    QColor getLineColor(){return lineColor;}

protected:
    virtual QSGNode * updatePaintNode(QSGNode *, UpdatePaintNodeData *)override;
signals:
  void lineColorChanged();
public slots:
};

} // namespace GUI

#endif // COLORPLOT_H
