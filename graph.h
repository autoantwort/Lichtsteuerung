#ifndef GRAPH_H
#define GRAPH_H

#include <QQuickItem>

class Graph : public QQuickItem
{
    Q_OBJECT
    float data[2048];
    int size = 0;
    QColor lineColor = QColor(0,0,0);
    Q_PROPERTY(QColor lineColor READ getLineColor WRITE setLineColor NOTIFY lineColorChanged)
    static Graph * lastCreated;
    std::atomic_bool haveNewData;
public:
    Graph();
    ~Graph(){
        if(lastCreated==this)
            lastCreated = nullptr;
    }
    static Graph * getLast(){return lastCreated;}
    void showData(float* data, int size){
        this->size = std::min(2048,size);
        memcpy(this->data,data,this->size*sizeof (float));
        haveNewData.store(true);
    }
    void setLineColor(QColor c){lineColor=c;update();lineColorChanged();}
    QColor getLineColor(){return lineColor;}

protected:
    virtual QSGNode * updatePaintNode(QSGNode *, UpdatePaintNodeData *)override;
signals:
  void lineColorChanged();
public slots:
};

#endif // GRAPH_H
