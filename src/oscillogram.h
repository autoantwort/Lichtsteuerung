#ifndef OSCILLOGRAM_H
#define OSCILLOGRAM_H

#include <QQuickItem>
#include <queue>

class Oscillogram : public QQuickItem
{
    Q_OBJECT
    float data[2048];
    int size = 0;
    std::queue<float> lastMaxValues;
    float maxValuesSum=0;
    float scale = 1;
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
        lastMaxValues.push(*std::max_element(data,data+size));
        maxValuesSum+=lastMaxValues.back();
        if(lastMaxValues.size()>500){
            maxValuesSum-=lastMaxValues.front();
            lastMaxValues.pop();
        }
        scale = std::min<float>(250.f,height()/2-50)/(maxValuesSum/lastMaxValues.size());
        std::copy(data,data+this->size,this->data);

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
