#include "driver.h"
#include <QLibrary>
#include <QDebug>
#include "errornotifier.h"
#include "programm.h"
#include "dmxchannelfilter.h"
#include <QDir>
#include <cstring>
#ifdef Q_OS_WIN
#include <windows.h>
#endif

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
    // we have to load the library with the Windows API to handle dependecies in the same folder, see https://bugreports.qt.io/projects/QTBUG/issues/QTBUG-74001
    path.replace("/","\\");
    auto pathC = path.toStdString();
    auto handle = LoadLibraryExA(pathC.c_str(),nullptr, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
    if(handle == nullptr){
        auto errorCode = GetLastError();
        qDebug() << "Loading of lib failed, error code: "<<errorCode;
        if(errorCode == 126)
            ErrorNotifier::get()->newError("Fehler beim laden der DLL, Abhängigkeiten zu anderen DLLs konnten nicht geladen werden.");
        else{
            ErrorNotifier::get()->newError("Fehler beim laden der DLL. Error code: " + QString::number(errorCode));
        }
        return false;
    }
    auto func = GetProcAddress(handle,"getDriver");
    if(func == nullptr){
        auto errorCode = GetLastError();
        qDebug() << "Loading of lib failed, error code: "<<errorCode;
        ErrorNotifier::get()->newError("Fehler beim laden der Funktion \"getDriver\" aus der DLL, Fehlercode: " + QString::number(errorCode));
        return false;
    }
    getDriverFunc getDriver = reinterpret_cast<getDriverFunc>(func);
    //getDriverFunc getDriver =  reinterpret_cast<getDriverFunc>(QLibrary::resolve(path,"getDriver"));
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
