#ifndef COLORPLOT_H
#define COLORPLOT_H


#include <QQuickItem>
#include <list>
#include <mutex>

class Colorplot : public QQuickItem
{
    Q_OBJECT
    unsigned int blockSize = 0;
    std::list<volatile float*> dataBlocks;
    std::list<float*> freeBlocks;
    int currentBlockCounter = -1;
    QColor lineColor = QColor(0,0,0);
    Q_PROPERTY(QColor lineColor READ getLineColor WRITE setLineColor NOTIFY lineColorChanged)
    static Colorplot * lastCreated;
    std::atomic_bool haveNewData;
    const static unsigned int MAX_BLOCKS_COUNT = 900;
    std::mutex mutex;

public:
    Colorplot();
    ~Colorplot(){
        if(lastCreated==this)
            lastCreated = nullptr;
    }
    static Colorplot * getLast(){return lastCreated;}
    void startBlock(){
        mutex.lock();
        if (dataBlocks.size()>=MAX_BLOCKS_COUNT) {
            currentBlockCounter = 0;
            auto data = dataBlocks.front();
            dataBlocks.pop_front();
            dataBlocks.push_back(data);
        }else{
            currentBlockCounter = 0;
            auto data = new float[blockSize];
            memset(data,0,blockSize*sizeof(float));
            dataBlocks.push_back(data);
        }
    }
    void endBlock(){
        mutex.unlock();
    }
    void pushDataToBlock(float d){
        if (currentBlockCounter>=0) {
            dataBlocks.back()[currentBlockCounter] = d;//*0.4f;//std::pow(1+d*0.001,3)*20;
            ++currentBlockCounter;
            if(currentBlockCounter>=blockSize)
                currentBlockCounter=-1;
        }else
            qDebug()<<"Error : currentBlockCounter less then 0";
    }
    void setBlockSize(unsigned int size){
        if(size!=this->blockSize){
            for(const auto i : dataBlocks)
                delete i;
            for(const auto i : freeBlocks)
                delete i;
            dataBlocks.clear();
            freeBlocks.clear();
            currentBlockCounter = -1;
            blockSize=size;
        }
    }
    void setLineColor(QColor c){lineColor=c;update();lineColorChanged();}
    QColor getLineColor(){return lineColor;}

protected:
    virtual QSGNode * updatePaintNode(QSGNode *, UpdatePaintNodeData *)override;
signals:
  void lineColorChanged();
public slots:
};


#endif // COLORPLOT_H
