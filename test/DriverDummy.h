#ifndef DRIVER_DUMMY_H
#define DRIVER_DUMMY_H

#include "HardwareInterface.h"
#include <QDebug>
#include <iostream>


class DriverDummy : public AbstractHardwareInterface
{
    unsigned char data[512];
public:
    DriverDummy () {}

    virtual bool init(){qDebug()<<"initDriver";return true;}
    virtual bool isFunctioning(){return true;}

protected:

    virtual void setValuesDeviceDriver() {
        qDebug()<<"set values";
        setValues(data,512,time);
        auto out = qDebug();
        for (int var = 0; var < 20; ++var) {
            out << (int)*(data+var);
        }
    }
};



#endif /*DRIVER_DUMMY_H*/
