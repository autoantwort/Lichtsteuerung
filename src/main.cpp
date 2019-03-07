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
#include "codeeditorhelper.h"
#include "programms/programblock.h"
#include "programblockeditor.h"
#include "graph.h"
#include <QTimer>
#include "oscillogram.h"
#include "colorplot.h"
#include "audio/audiocapturemanager.h"
#include "test/testsampleclass.h"

int main(int argc, char *argv[])
{
    /*Test::TestModulSystem testModulSystem;
    testModulSystem.runTest();
    return 0;*/
    // init the rand function for different random numbers each startup
    srand(static_cast<unsigned int>(time(nullptr)));

    Test::testSampleClass();

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
    // normally this should be done automatically
    qRegisterMetaType<QAbstractListModel*>("QAbstractListModel*");
    qRegisterMetaType<PropertyInformationModel*>("PropertyInformationModel*");
    //qmlRegisterType<const ChannelVector*>("my.models",1,0,"ChannelVector");
    qmlRegisterType<ChannelProgrammEditor>("custom.licht",1,0,"ChannelProgrammEditor");
    qmlRegisterType<GUI::MapView>("custom.licht",1,0,"MapView");
    qmlRegisterType<GUI::MapEditor>("custom.licht",1,0,"MapEditor");
    qmlRegisterType<ControlPanel>("custom.licht",1,0,"ControlPanel");
    qmlRegisterType<Graph>("custom.licht",1,0,"Graph");
    qmlRegisterType<Oscillogram>("custom.licht",1,0,"Oscillogram");
    qmlRegisterType<Colorplot>("custom.licht",1,0,"Colorplot");
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
    qRegisterMetaType<Modules::ValueType>("ValueType");
    qRegisterMetaType<Modules::ProgramBlock::Status>("Status");
    qRegisterMetaType<Modules::PropertiesVector*>("PropertiesVector*");

    // Load Settings and ApplicationData
    Settings::setLocalSettingFile(QFileInfo("settings.ini"));
    Settings settings;
    QFile file("QTJSONFile.json");
    if(!file.exists()){
        file.setFileName(settings.getJsonSettingsFilePath());
    }
    if(file.exists()){
        file.copy(file.fileName()+"_"+QDateTime::currentDateTime().toString("dd.MM.yyyy HH.mm.ss"));
    }
    auto after = ApplicationData::loadData(file);

//#warning Dont use IDBase<xxxxx>::getAllIDBases() in this file. It will crash the aplication when its closing

    std::thread t(Test::testLoopProgramm);
    t.join();



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
    settings.connect(&settings,&Settings::audioCaptureFilePathChanged,[&](){
        if(Audio::AudioCaptureManager::get().startCapturing(settings.getAudioCaptureFilePath())==false){
            ErrorNotifier::get()->newError("Failed to load Audio Capture Library");
        }
    });
    settings.connect(&settings,&Settings::updatePauseInMsChanged,[&](){
        if(Driver::getCurrentDriver()){
            Driver::getCurrentDriver()->setWaitTime(std::chrono::milliseconds(settings.getUpdatePauseInMs()));
        }
    });
    Modules::ModuleManager::singletone()->loadAllModulesInDir(settings.getModuleDirPath());
    settings.connect(&settings,&Settings::moduleDirPathChanged,[&](){
        Modules::ModuleManager::singletone()->loadAllModulesInDir(settings.getModuleDirPath());
    });


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
    }else {
        Driver::getCurrentDriver()->setWaitTime(std::chrono::milliseconds(40));
    }

#else
#include "test/DriverDummy.h"

    /*DriverDummy driver;
    driver.setSetValuesCallback([](unsigned char* values, int size, double time){
        std::memset(values,0,size);
        DMXChannelFilter::initValues(values,size);
        Programm::fill(values,size,time);
        Modules::DMXConsumer::fillWithDMXConsumer(values,size);
        DMXChannelFilter::filterValues(values,size);
    });
    driver.setWaitTime(std::chrono::seconds(5));
    driver.init();
    driver.start();*/
#endif

    QTimer timer;
    timer.setInterval(15);
    QObject::connect(&timer,&QTimer::timeout,[&](){
        if(Audio::AudioCaptureManager::get().isCapturing()){
            if(Graph::getLast())
                Graph::getLast()->update();
            if(Oscillogram::getLast())
                Oscillogram::getLast()->update();
            if(Colorplot::getLast())
                Colorplot::getLast()->update();
        }
    });
    timer.start();

    qDebug() << "start capturing : " << Audio::AudioCaptureManager::get().startCapturing(settings.getAudioCaptureFilePath());

    Modules::ModuleManager::singletone()->controller().start();
    //ControlPanel::getLastCreated()->addDimmerGroupControl();
    return app.exec();
}
