#include "applicationdata.h"
#include "codeeditorhelper.h"
#include "errornotifier.h"
#include "modelmanager.h"
#include "settings.h"
#include "sortedmodelview.h"
#include "updater.h"
#include "usermanagment.h"
#include "audio/audiocapturemanager.h"
#include "dmx/HardwareInterface.h"
#include "dmx/channel.h"
#include "dmx/device.h"
#include "dmx/dmxchannelfilter.h"
#include "dmx/driver.h"
#include "dmx/programm.h"
#include "gui/audioeventdataview.h"
#include "gui/channelprogrammeditor.h"
#include "gui/colorplot.h"
#include "gui/controlitem.h"
#include "gui/controlitemdata.h"
#include "gui/controlpanel.h"
#include "gui/graph.h"
#include "gui/mapeditor.h"
#include "gui/mapview.h"
#include "gui/oscillogram.h"
#include "gui/programblockeditor.h"
#include "modules/dmxconsumer.h"
#include "modules/ledconsumer.h"
#include "modules/programblock.h"
#include "spotify/spotify.h"
#include "system_error_handler.h"
#include "test/testloopprogramm.h"
#include "test/testmodulsystem.h"
#include "test/testsampleclass.h"
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QEasingCurve>
#include <QFileInfo>
#include <QGuiApplication>
#include <QLibrary>
#include <QMetaProperty>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickView>
#include <QSharedMemory>
#include <QSslSocket>
#include <QTimer>
#include <chrono>
#include <cmath>
#include <id.h>
#include <limits>

#ifdef Q_OS_WIN
#include <winuser.h>
#endif

#ifdef DrMinGW
#include "exchndl.h"
#include <QNetworkReply>
#endif

int main(int argc, char *argv[]) {
    QSharedMemory mem(QStringLiteral("Lichteuerung Leander Schulten"));
    if (!mem.create(1)) {
#ifdef Q_OS_WIN
        MessageBoxA(nullptr, "The Lichtsteuerung is already running on this computer.", nullptr, MB_OK);
#endif
        return 0;
    }
    error::initErrorHandler();
#ifdef DrMinGW
    ExcHndlInit();
    auto path = QStandardPaths::writableLocation(QStandardPaths::QStandardPaths::AppDataLocation);
    path += QLatin1String("/Lichtsteuerung");
    QDir dir(path);
    if(!dir.mkpath(path)){
        qWarning() << "Error creating dirs : " << path;
    }
    path += QLatin1String("/crash_dump_");
    path += QDateTime::currentDateTime().toString(QStringLiteral("dd.MM.yyyy HH.mm.ss"));
    path += QLatin1String(".txt");
    qDebug() << "The crash report file is : " << path;
    ExcHndlSetLogFileNameA(path.toStdString().c_str());
#endif

    if (!QSslSocket::supportsSsl()) {
        ErrorNotifier::showError(QStringLiteral("No OpenSSL library found!\nUpdates are not possible and the Spotify support does not work."));
    }

    Updater updater;
    QObject::connect(&updater,&Updater::needUpdate,[&](){
        updater.update();
    });
#ifdef DrMinGW
    // send crash reports
    auto files = dir.entryInfoList(QDir::Filter::Files);
    for(auto & file : files){
        if(file.fileName().startsWith(QLatin1String("crash_dump"))){
            auto newFileName = file.absolutePath() + "/sended_" +file.fileName();
            if(!QFile::rename(file.absoluteFilePath(),newFileName)){
                qWarning() << "Failed to rename file from " << file.absoluteFilePath() << " to " << newFileName;
                continue;
            }
            auto upload = [newFileName,&updater](){
                // we only want to report Bugs for new versions
                if(updater.getState() == Updater::UpdaterState::NoUpdateAvailible){
                    QFile file(newFileName);
                    if(!file.open(QIODevice::ReadOnly)){
                        qWarning() <<  "Failed to open file " << newFileName;
                        return;
                    }
                    auto request = QNetworkRequest(QUrl(QStringLiteral("https://orga.symposion.hilton.rwth-aachen.de/send_crash_report")));
                    request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader,"text/plain");
                    auto response = updater.getQNetworkAccessManager()->post(request,file.readAll());
                    QObject::connect(response,&QNetworkReply::finished,[=](){
                        if(response->error() == QNetworkReply::NoError){
                            qDebug() << "crash report erfolgreich hochlgeladen";
                        }else{
                            qWarning() << "Fehler beim hochladen des crash reports : " << response->errorString();
                        }
                        response->deleteLater();
                    });
                }
            };
            if(updater.getState() == Updater::UpdaterState::NotChecked){
                QObject::connect(&updater,&Updater::stateChanged,upload);
            }else{
                upload();
            }
        }
    }
#endif
    /*Test::TestModulSystem testModulSystem;
    testModulSystem.runTest();
    return 0;*/
    // init the rand function for different random numbers each startup
    srand(static_cast<unsigned int>(time(nullptr)));

    Test::testSampleClass();

    class CatchingErrorApplication : public QGuiApplication{
    public:
        CatchingErrorApplication(int &argc, char **argv):QGuiApplication(argc,argv){}
        bool notify(QObject* receiver, QEvent* event) override
        {
            try {
                return QGuiApplication::notify(receiver, event);
            } catch (std::exception &e) {
                qCritical("Error %s sending event %s to object %s (%s)",
                    e.what(), typeid(*event).name(), qPrintable(receiver->objectName()),
                    receiver->metaObject()->className());
                QString error = QStringLiteral("Error ") +e.what()+"sending event "+typeid(*event).name()+" to object "+ qPrintable(receiver->objectName())+" "+ receiver->metaObject()->className();
                ErrorNotifier::get()->newError(error);
            } catch (...) {
                qCritical("Error <unknown> sending event %s to object %s (%s)",
                    typeid(*event).name(), qPrintable(receiver->objectName()),
                    receiver->metaObject()->className());
                QString error = QStringLiteral("Error ") + "<unknown> sending event "+typeid(*event).name()+" to object "+ qPrintable(receiver->objectName())+" "+ receiver->metaObject()->className();
                ErrorNotifier::get()->newError(error);
            }

            return false;
        }
    };

    using namespace GUI;
    using namespace DMX;

//    auto defaultFormat = QSurfaceFormat::defaultFormat();
//    defaultFormat.setSamples(8);
//    QSurfaceFormat::setDefaultFormat(defaultFormat);
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    CatchingErrorApplication app(argc, argv);
    QQmlApplicationEngine engine;
    ControlPanel::setQmlEngine(&engine);
    ProgramBlockEditor::engine = &engine;
    Driver::dmxValueModel.setQMLEngineThread(engine.thread());
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
    qmlRegisterType<GUI::AudioEventDataView>("custom.licht", 1, 0, "AudioEventDataView");
    qmlRegisterType<SortedModelVectorView>("custom.licht",1,0,"SortedModelVectorView");
    qRegisterMetaType<DMXChannelFilter::Operation>("Operation");
    qmlRegisterUncreatableType<UserManagment>("custom.licht",1,0,"Permission",QStringLiteral("Singletone in c++"));
    qmlRegisterUncreatableMetaObject(Updater::staticMetaObject,"custom.licht",1,0,"UpdaterState",QStringLiteral("Enum in c++"));
    qmlRegisterUncreatableMetaObject(ControlItemData::staticMetaObject,"custom.licht",1,0,"ControlType",QStringLiteral("Enum in c++"));
    qRegisterMetaType<UserManagment::Permission>("Permission");
    qRegisterMetaType<Modules::detail::PropertyInformation::Type>("Type");
    qRegisterMetaType<Modules::ValueType>("ValueType");
    qRegisterMetaType<Modules::ProgramBlock::Status>("Status");
    qRegisterMetaType<Modules::PropertiesVector*>("PropertiesVector*");
    qRegisterMetaType<Driver::DMXQMLValue*>("DMXQMLValue*");
    qRegisterMetaType<DMX::DMXChannelFilter*>("DMXChannelFilter*");


    updater.checkForUpdate();

    // Load Settings and ApplicationData
    Settings::setLocalSettingFile(QFileInfo(QStringLiteral("settings.ini")));
    Settings settings;
    QFile file(settings.getJsonSettingsFilePath());
    if(!file.exists()){
        file.setFileName(QStringLiteral("QTJSONFile.json"));
    }
    if(file.exists()){
        file.copy(file.fileName()+"_"+QDateTime::currentDateTime().toString(QStringLiteral("dd.MM.yyyy HH.mm.ss")));
    } else {
        ErrorNotifier::showError(QStringLiteral("No settings file found! The Lichtsteuerung starts wihout content."));
    }
    auto after = ApplicationData::loadData(file);
    // nachdem die Benutzer geladen wurden, auto login durchführen
    UserManagment::get()->autoLoginUser();

//#warning Dont use IDBase<xxxxx>::getAllIDBases() in this file. It will crash the aplication when its closing

    std::thread t(Test::testLoopProgramm);
    t.join();

    auto & spotify = Spotify::get();
    Modules::ModuleManager::singletone()->setSpotify(&spotify);
    spotify.autoLoginUser();

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
    valueTypeList << QStringLiteral("Brightness") << QStringLiteral("RGB") ;

    QStringList modolePropertyTypeList;
    modolePropertyTypeList << QStringLiteral("Int") << QStringLiteral("Long") << QStringLiteral("Float") << QStringLiteral("Double") << QStringLiteral("Bool") << QStringLiteral("String") << QStringLiteral("RGB Color");
    // Does not work: do it manually
    /*const QMetaObject &_momProp = Modules::detail::PropertyInformation::staticMetaObject;
    qDebug() << "Enum count" <<_momProp.enumeratorCount();
    QMetaEnum _metaEnumP =_momProp.enumerator(mo.indexOfEnumerator("Type"));
    for (int i = 0; i < _metaEnumP.keyCount(); ++i) {
        modolePropertyTypeList.append(_metaEnumP.key(i));
    }*/

    CatchingErrorApplication::connect(&app, &QGuiApplication::lastWindowClosed, [&]() {
        Modules::ModuleManager::singletone()->controller().stop();
        Audio::AudioCaptureManager::get().stopCapturingAndWait();
        QFile savePath(settings.getJsonSettingsFilePath());
        ApplicationData::saveData(savePath);
        Driver::stopAndUnloadDriver();
        updater.runUpdateInstaller();
    });
    Settings::connect(&settings,&Settings::driverFilePathChanged,[&](){
        Driver::loadAndStartDriver(settings.getDriverFilePath());
    });
    Settings::connect(&settings,&Settings::updatePauseInMsChanged,[&](){
        if(Driver::getCurrentDriver()){
            Driver::getCurrentDriver()->setWaitTime(std::chrono::milliseconds(settings.getUpdatePauseInMs()));
        }
    });
    Modules::ModuleManager::singletone()->loadAllModulesInDir(settings.getModuleDirPath());
    Settings::connect(&settings,&Settings::moduleDirPathChanged,[&](){
        Modules::ModuleManager::singletone()->loadAllModulesInDir(settings.getModuleDirPath());
    });

    ModelManager::get().setSettings(&settings);
    engine.rootContext()->setContextProperty(QStringLiteral("ModelManager"),&ModelManager::get());
    engine.rootContext()->setContextProperty(QStringLiteral("easingModel"),dataList);
    engine.rootContext()->setContextProperty(QStringLiteral("ErrorNotifier"),ErrorNotifier::get());
    QQmlApplicationEngine::setObjectOwnership(ErrorNotifier::get(),QQmlEngine::CppOwnership);
    engine.rootContext()->setContextProperty(QStringLiteral("devicePrototypeModel"),ModelManager::get().getDevicePrototypeModel());
    engine.rootContext()->setContextProperty(QStringLiteral("modulesModel"),Modules::ModuleManager::singletone()->getModules());
    engine.rootContext()->setContextProperty(QStringLiteral("moduleTypeModel"),moduleTypeList);
    engine.rootContext()->setContextProperty(QStringLiteral("valueTypeList"),valueTypeList);
    engine.rootContext()->setContextProperty(QStringLiteral("modolePropertyTypeList"),modolePropertyTypeList);
    engine.rootContext()->setContextProperty(QStringLiteral("deviceModel"),ModelManager::get().getDeviceModel());
    engine.rootContext()->setContextProperty(QStringLiteral("programmModel"),ModelManager::get().getProgramModel());
    engine.rootContext()->setContextProperty(QStringLiteral("programmPrototypeModel"),ModelManager::get().getProgramPrototypeModel());
    engine.rootContext()->setContextProperty(QStringLiteral("programBlocksModel"),&Modules::ProgramBlockManager::model);
    engine.rootContext()->setContextProperty(QStringLiteral("userModel"),UserManagment::get()->getUserModel());
    engine.rootContext()->setContextProperty(QStringLiteral("UserManagment"),UserManagment::get());
    engine.rootContext()->setContextProperty(QStringLiteral("Settings"),&settings);
    engine.rootContext()->setContextProperty(QStringLiteral("spotify"),&spotify);
    engine.rootContext()->setContextProperty(QStringLiteral("updater"),&updater);
    engine.rootContext()->setContextProperty(QStringLiteral("ledConsumer"),&Modules::LedConsumer::allLedConsumer);
    QQmlEngine::setObjectOwnership(&Driver::dmxValueModel,QQmlEngine::CppOwnership);
    engine.rootContext()->setContextProperty(QStringLiteral("dmxOutputValues"),&Driver::dmxValueModel);
    engine.rootContext()->setContextProperty(QStringLiteral("AudioManager"), &Audio::AudioCaptureManager::get());
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));


    // laden erst nach dem laden des qml ausführen
    after();


    // Treiber laden
#define USE_DUMMY_DRIVER
#ifndef USE_DUMMY_DRIVER
    if (!Driver::loadAndStartDriver(settings.getDriverFilePath())) {
        ErrorNotifier::showError(QStringLiteral("Can`t start the DMX driver. The DMX output will not work. You can load a different driver in the Settings tab."));
    } else {
        Driver::getCurrentDriver()->setWaitTime(std::chrono::milliseconds(40));
    }

#else
#include "test/DriverDummy.h"

    DriverDummy driver;
    driver.setSetValuesCallback([](unsigned char* values, int size, double time){
        std::memset(values,0,size);
        DMXChannelFilter::initValues(values,size);
        DMX::Programm::fill(values,size,time);
        Modules::DMXConsumer::fillWithDMXConsumer(values,size);
        DMXChannelFilter::filterValues(values,size);
        Driver::dmxValueModel.setValues(values,size);
    });
    driver.setWaitTime(std::chrono::milliseconds(40));
    driver.init();
    driver.start();
#endif

    auto &audioManager = Audio::AudioCaptureManager::get();
    if (!audioManager.startCapturingFromDefaultInput()) {
        ErrorNotifier::showError("Audio capturing not possible");
    }

    Modules::ModuleManager::singletone()->controller().start();
    //ControlPanel::getLastCreated()->addDimmerGroupControl();
    return CatchingErrorApplication::exec();
}
