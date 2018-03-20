#include <QCoreApplication>
#include <QDebug>
#include <id.h>
#include <device.h>
#include "channel.h"
#include "applicationdata.h"
#include "programm.h"
#include <QMetaProperty>
#include "dmxchannelfilter.h"
#include <chrono>
#include <QEasingCurve>
#include <cmath>
#include <QCryptographicHash>
#include "usermanagment.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "device.h"
#include <QQmlContext>
#include <QFileInfo>
#include <limits>
#include "modelmanager.h"
#include "channelprogrammeditor.h"
#include "mapview.h"
#include "mapeditor.h"
#include "controlitem.h"
#include "controlpanel.h"
#include "errornotifier.h"
#include <QQuickView>
#include <QLibrary>
#include "HardwareInterface.h"
#include "settings.h"
#include <QDir>
#include "driver.h"

int main(int argc, char *argv[])
{


    class CatchingErrorApplication : public QGuiApplication{
    public:
        CatchingErrorApplication(int &argc, char **argv):QGuiApplication(argc,argv){}
        virtual ~CatchingErrorApplication(){}
        virtual bool notify(QObject* receiver, QEvent* event)
        {
            try {
                return QGuiApplication::notify(receiver, event);
            } catch (std::exception &e) {
                qCritical("Error %s sending event %s to object %s (%s)",
                    e.what(), typeid(*event).name(), qPrintable(receiver->objectName()),
                    receiver->metaObject()->className());
                QString error = QString("Error ") +e.what()+"sending event "+typeid(*event).name()+" to object "+ qPrintable(receiver->objectName())+" "+ receiver->metaObject()->className();
                ErrorNotifier::get()->newError(error);
            } catch (...) {
                qCritical("Error <unknown> sending event %s to object %s (%s)",
                    typeid(*event).name(), qPrintable(receiver->objectName()),
                    receiver->metaObject()->className());
                QString error = QString("Error ") + "<unknown> sending event "+typeid(*event).name()+" to object "+ qPrintable(receiver->objectName())+" "+ receiver->metaObject()->className();
                ErrorNotifier::get()->newError(error);
            }

            return false;
        }
    };



//    auto defaultFormat = QSurfaceFormat::defaultFormat();
//    defaultFormat.setSamples(8);
//    QSurfaceFormat::setDefaultFormat(defaultFormat);
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    CatchingErrorApplication app(argc, argv);
    QQmlApplicationEngine engine;
    ControlPanel::setQmlEngine(&engine);
    //qmlRegisterType<const ChannelVector*>("my.models",1,0,"ChannelVector");
    qmlRegisterType<ChannelProgrammEditor>("custom.licht",1,0,"ChannelProgrammEditor");
    qmlRegisterType<MapView>("custom.licht",1,0,"MapView");
    qmlRegisterType<MapEditor>("custom.licht",1,0,"MapEditor");
    qmlRegisterType<ControlPanel>("custom.licht",1,0,"ControlPanel");
    //qmlRegisterType<ErrorNotifier>("custom.licht",1,0,"ErrorNotifier");
    qmlRegisterType<ControlItem>("custom.licht.template",1,0,"ControlItemTemplate");
    qmlRegisterType<ControlItemData>("custom.licht.template",1,0,"ControlItemData");
    qmlRegisterType<DimmerGroupControlItemData>("custom.licht",1,0,"DimmerGroupControlItemData");
    qmlRegisterType<DMXChannelFilter>("custom.licht",1,0,"DMXChannelFilter");
    qRegisterMetaType<DMXChannelFilter::Operation>("Operation");
    qmlRegisterUncreatableType<UserManagment>("custom.licht",1,0,"Permission","Singletone in c++");
    qRegisterMetaType<UserManagment::Permission>("Permission");
    Settings settings;
    settings.setJsonSettingsFilePath("QTJSONFile.json");
    QFile file(settings.getJsonSettingsFilePath());
    file.copy(settings.getJsonSettingsFilePath()+"_"+QDateTime::currentDateTime().toString("dd.MM.YYYY HH:mm:ss"));

//#warning Dont use IDBase<xxxxx>::getAllIDBases() in this file. It will crash the aplication when its closing

    auto after = ApplicationData::loadData(file);


    QStringList dataList;
    const QMetaObject &mo = QEasingCurve::staticMetaObject;
    QMetaEnum metaEnum = mo.enumerator(mo.indexOfEnumerator("Type"));
    for (int i = 0; i < QEasingCurve::NCurveTypes - 1; ++i) {
        dataList.append(metaEnum.key(i));
    }

    app.connect(&app,&QGuiApplication::lastWindowClosed,[&](){
        QFile savePath(settings.getJsonSettingsFilePath());
        ApplicationData::saveData(savePath);
        Driver::stopAndUnloadDriver();
    });
    settings.connect(&settings,&Settings::driverFilePathChanged,[&](){
        Driver::loadAndStartDriver(settings.getDriverFilePath());
    });
    settings.connect(&settings,&Settings::updatePauseInMsChanged,[&](){
        if(Driver::getCurrentDriver()){
            Driver::getCurrentDriver()->setWaitTime(std::chrono::milliseconds(settings.getUpdatePauseInMs()));
        }
    });


    engine.rootContext()->setContextProperty("ModelManager",new ModelManager());
    engine.rootContext()->setContextProperty("easingModel",dataList);
    engine.rootContext()->setContextProperty("ErrorNotifier",ErrorNotifier::get());
    engine.setObjectOwnership(ErrorNotifier::get(),QQmlEngine::CppOwnership);
    engine.rootContext()->setContextProperty("devicePrototypeModel",IDBaseDataModel<DevicePrototype>::singletone());
    engine.rootContext()->setContextProperty("deviceModel",IDBaseDataModel<Device>::singletone());
    engine.rootContext()->setContextProperty("programmModel",IDBaseDataModel<Programm>::singletone());
    engine.rootContext()->setContextProperty("programmPrototypeModel",IDBaseDataModel<ProgrammPrototype>::singletone());
    engine.rootContext()->setContextProperty("userModel",IDBaseDataModel<User>::singletone());
    engine.rootContext()->setContextProperty("UserManagment",UserManagment::get());
    engine.rootContext()->setContextProperty("Settings",&settings);
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));


    // laden erst nach dem laden des qml ausführen
    after();


    // Treiber laden
//#define USE_DUMMY_DRIVER
#ifndef USE_DUMMY_DRIVER
    if(!Driver::loadAndStartDriver(settings.getDriverFilePath())){
        ErrorNotifier::showError("Cant start driver.");
    }
    Driver::getCurrentDriver()->setWaitTime(std::chrono::milliseconds(15));
#else
#include "test/DriverDummy.h"

    DriverDummy driver;
    driver.setSetValuesCallback([](unsigned char* values, int size, double time){
        DMXChannelFilter::initValues(values,size);
        Programm::fill(values,size,time);
        DMXChannelFilter::filterValues(values,size);
    });
    driver.setWaitTime(std::chrono::seconds(5));
    driver.init();
    driver.start();
#endif


    //ControlPanel::getLastCreated()->addDimmerGroupControl();
    return app.exec();
}
