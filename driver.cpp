#include "driver.h"
#include <QLibrary>
#include <QDebug>
#include "errornotifier.h"
#include "programm.h"
#include "dmxchannelfilter.h"
#include <QDir>
#include <cstring>

HardwareInterface * driver = nullptr;

namespace Driver {

    bool loadAndStartDriver(QString path){
        if(!loadDriver(path)){
            return false;
        }
        if(driver){
            if(!driver->init())
                return false;
            driver->start();
            return true;
        }
        return false;
    }

    bool loadDriver(QString path){
#if defined(Q_OS_WIN)
    typedef HardwareInterface * (*getDriverFunc)();
    getDriverFunc getDriver =  reinterpret_cast<getDriverFunc>(QLibrary::resolve(path,"getDriver"));
    if(getDriver!=nullptr){
        HardwareInterface * inter = getDriver();
        if(inter != nullptr){
            inter->setErrorCallback([](QString s){
                qDebug()<<s;
                ErrorNotifier::get()->newError(s);
            });
            inter->setSetValuesCallback([](unsigned char* values, int size, double time){
                std::memset(values,0,size);
                DMXChannelFilter::initValues(values,size);
                Programm::fill(values,size,time);
                DMXChannelFilter::filterValues(values,size);
            });

            if(driver){
                driver->stop();
                delete driver;
            }
            driver=inter;
            return true;
        }
    }else{
        if(!QFile::exists(path)){
            ErrorNotifier::get()->newError("Driver File not existing : " + path + '\n' + "Home dir is : " + QDir::currentPath());
        }else{
            ErrorNotifier::get()->newError("Can not resolve Function getDriver in Library at Location "+path);
        }
    }

#else
#warning Driverloading is only supported for Windows
#endif
        return false;
    }

    HardwareInterface* getCurrentDriver(){
        return driver;
    }

    void stopAndUnloadDriver(){
        if (driver) {
            driver->stop();
            delete driver;
        }
    }
}
