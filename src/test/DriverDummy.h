#ifndef DRIVER_DUMMY_H
#define DRIVER_DUMMY_H

#include "dmx/HardwareInterface.h"
#include <QDebug>
#include <iostream>


class DriverDummy : public DMX::AbstractHardwareInterface
{
    unsigned char data[512];
public:
    DriverDummy () {}
virtual ~DriverDummy(){}
    virtual bool init()override final {qDebug()<<"initDriver";return true;}
    virtual bool isFunctioning()override final {return true;}

protected:

    virtual void setValuesDeviceDriver() override final{
        setValues(data,512,time);
    }
};



#endif /*DRIVER_DUMMY_H*/
