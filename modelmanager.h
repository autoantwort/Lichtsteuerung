#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include <QObject>
#include "device.h"

class ModelManager : public QObject{
    Q_OBJECT
public:
    Q_INVOKABLE void remove(QObject * item){
        if(item)
            delete item;
    }
    Q_INVOKABLE bool addDevice(int row, int startDMXChannel, QString name, QString desciption="",QPoint position = QPoint(-1,-1)){
        qDebug()<<"addDevice : "<<row<<' ' << startDMXChannel<< ' ' << name << ' '<<desciption<<'\n';
        DevicePrototype * prototype = IDBaseDataModel<DevicePrototype>::singletone()->data(row);
        if(prototype){
            new Device(prototype,startDMXChannel,name,desciption,position);
            return true;
        }
        return false;
    }
};

#endif // MODELMANAGER_H
