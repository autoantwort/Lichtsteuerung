#ifndef OSCILLOGRAM_H
#define OSCILLOGRAM_H

#include <QQuickItem>

class Oscillogram : public QQuickItem
{
    Q_OBJECT
    float data[2048];
    int size = 0;
    QColor lineColor = QColor(0,0,0);
    Q_PROPERTY(QColor lineColor READ getLineColor WRITE setLineColor NOTIFY lineColorChanged)
    static Oscillogram * lastCreated;
    std::atomic_bool haveNewData;
public:
    Oscillogram();
    ~Oscillogram(){
        if(lastCreated==this)
            lastCreated = nullptr;
    }
    static Oscillogram * getLast(){return lastCreated;}
    void showData(float* data, int size){
        this->size = std::min(2048,size);
        memcpy(this->data,data,this->size);
        haveNewData.store(true);
    }
    void showData(volatile float* data, int size){
        this->size = std::min(2048,size);
        for(int i = 0; i<this->size;++i){
            this->data[i] = data[i];
        }
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

#endif // OSCILLOGRAM_H
