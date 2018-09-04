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
#include "test/testloopprogramm.h"
#include "test/testmodulsystem.h"
#include "test/testmodulecontroller.h"
#include "test/testprogrammblock.h"
#include "codeeditorhelper.h"
#include "programms/programblock.h"
#include "programblockeditor.h"

namespace Modules {
}

int main(int argc, char *argv[])
{
    /*Test::TestModulSystem testModulSystem;
    testModulSystem.runTest();
    return 0;*/
    //Modules::rgb_t t = {{{{1},{4},{3}}}};
    Test::createProgrammBlockAndTestRunIt();

    /*{
        using namespace std;
        using namespace string_literals;
        vector<shared_ptr<string>> vec;
        vec.push_back(make_shared<string>("Hallo"));
        vec.front()->append(" Wourld"s);
        cout << vec.front().get() << endl;
        vec[0] = make_shared<string>("TEST"s);
        cout << vec.front().get() << endl;
        shared_ptr<string> second = make_shared<string>("Second"s);
        vec[0].swap(second);
        cout << vec.front().get() << endl;


    }*/




//    Test::TestProgrammBlock().test();
//    return 0;

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
    ProgramBlockEditor::engine = &engine;
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
    qmlRegisterType<CodeEditorHelper>("custom.licht",1,0,"CodeEditorHelper");
    qmlRegisterType<ProgramBlockEditor>("custom.licht",1,0,"ProgramBlockEditor");
    qRegisterMetaType<DMXChannelFilter::Operation>("Operation");
    qmlRegisterUncreatableType<UserManagment>("custom.licht",1,0,"Permission","Singletone in c++");
    qRegisterMetaType<UserManagment::Permission>("Permission");
    qRegisterMetaType<Modules::detail::PropertyInformation::Type>("Type");
    //qmlRegisterType<Modules::detail::PropertyInformation::Type>("custom.licht", 1, 0, "PropertyType");
    //qmlRegisterUncreatableMetaObject(Modules::detail::PropertyInformation::staticMetaObject,"test",1,0,"we","no enum");
    //qmlRegisterUncreatableMetaObject(Modules::Property::Type,"test",1,0,"we","no enum");
    qRegisterMetaType<Modules::ValueType>("ValueType");
    qRegisterMetaType<Modules::PropertiesVector*>("PropertiesVector*");
    Settings settings;
    settings.setJsonSettingsFilePath("QTJSONFile.json");
    QFile file(settings.getJsonSettingsFilePath());
    file.copy(settings.getJsonSettingsFilePath()+"_"+QDateTime::currentDateTime().toString("dd.MM.YYYY HH:mm:ss"));

//#warning Dont use IDBase<xxxxx>::getAllIDBases() in this file. It will crash the aplication when its closing

    std::thread t(test);
    t.join();

    auto after = ApplicationData::loadData(file);


    QStringList dataList;
    const QMetaObject &mo = QEasingCurve::staticMetaObject;
    QMetaEnum metaEnum = mo.enumerator(mo.indexOfEnumerator("Type"));
    for (int i = 0; i < QEasingCurve::NCurveTypes - 1; ++i) {
        dataList.append(metaEnum.key(i));
    }
    QStringList moduleTypeList;
    const QMetaObject &_mom = Modules::Module::staticMetaObject;
    QMetaEnum _metaEnum =_mom.enumerator(_mom.indexOfEnumerator("Type"));
    for (int i = 0; i < _metaEnum.keyCount(); ++i) {
        moduleTypeList.append(_metaEnum.key(i));
    }
    QStringList valueTypeList;
    valueTypeList << "Brightness" << "RGB" ;

    QStringList modolePropertyTypeList;
    modolePropertyTypeList << "Int" << "Long" << "Float" << "Double" << "Bool" << "String";
    // Does not work: do it manually
    /*const QMetaObject &_momProp = Modules::detail::PropertyInformation::staticMetaObject;
    qDebug() << "Enum count" <<_momProp.enumeratorCount();
    QMetaEnum _metaEnumP =_momProp.enumerator(mo.indexOfEnumerator("Type"));
    for (int i = 0; i < _metaEnumP.keyCount(); ++i) {
        modolePropertyTypeList.append(_metaEnumP.key(i));
    }*/

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
    Modules::ModuleManager::singletone()->loadAllModulesInDir(settings.getModuleDirPath());
    settings.connect(&settings,&Settings::moduleDirPathChanged,[&](){
        Modules::ModuleManager::singletone()->loadAllModulesInDir(settings.getModuleDirPath());
    });/*
    auto mm = Modules::ModuleManager::singletone();
    qDebug()<<"Loaded : ";
    for(auto m : mm->getConsumerModules()){
        qDebug() << "Consumer : "<<QString::fromStdString(m.name());
    }
    for(auto m : mm->getProgrammModules()){
        qDebug() << "Program : "<<QString::fromStdString(m.name());
    }
    for(auto m : mm->getFilterModules()){
        qDebug() << "Filter : "<<QString::fromStdString(m.name());
    }*/


    engine.rootContext()->setContextProperty("ModelManager",new ModelManager());
    engine.rootContext()->setContextProperty("easingModel",dataList);
    engine.rootContext()->setContextProperty("ErrorNotifier",ErrorNotifier::get());
    engine.setObjectOwnership(ErrorNotifier::get(),QQmlEngine::CppOwnership);
    engine.rootContext()->setContextProperty("devicePrototypeModel",IDBaseDataModel<DevicePrototype>::singletone());
    engine.rootContext()->setContextProperty("modulesModel",Modules::ModuleManager::singletone()->getModules());
    engine.rootContext()->setContextProperty("moduleTypeModel",moduleTypeList);
    engine.rootContext()->setContextProperty("valueTypeList",valueTypeList);
    engine.rootContext()->setContextProperty("modolePropertyTypeList",modolePropertyTypeList);
    engine.rootContext()->setContextProperty("deviceModel",IDBaseDataModel<Device>::singletone());
    engine.rootContext()->setContextProperty("programmModel",IDBaseDataModel<Programm>::singletone());
    engine.rootContext()->setContextProperty("programmPrototypeModel",IDBaseDataModel<ProgrammPrototype>::singletone());
    engine.rootContext()->setContextProperty("programBlocksModel",&Modules::ProgramBlockManager::model);
    engine.rootContext()->setContextProperty("userModel",IDBaseDataModel<User>::singletone());
    engine.rootContext()->setContextProperty("UserManagment",UserManagment::get());
    engine.rootContext()->setContextProperty("Settings",&settings);
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));


    // laden erst nach dem laden des qml ausführen
    after();


    // Treiber laden
#define USE_DUMMY_DRIVER
#ifndef USE_DUMMY_DRIVER
    if(!Driver::loadAndStartDriver(settings.getDriverFilePath())){
        ErrorNotifier::showError("Cant start driver.");
    }
    Driver::getCurrentDriver()->setWaitTime(std::chrono::milliseconds(15));
#else
#include "test/DriverDummy.h"

    /*DriverDummy driver;
    driver.setSetValuesCallback([](unsigned char* values, int size, double time){
        DMXChannelFilter::initValues(values,size);
        Programm::fill(values,size,time);
        DMXChannelFilter::filterValues(values,size);
    });
    driver.setWaitTime(std::chrono::seconds(5));
    driver.init();
    driver.start();*/
#endif

    Test::createProgrammBlockAndTestRunIt();
    //ControlPanel::getLastCreated()->addDimmerGroupControl();
    return app.exec();
}
