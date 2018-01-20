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
#include <QQuickView>
int main(int argc, char *argv[])
{

//    auto defaultFormat = QSurfaceFormat::defaultFormat();
//    defaultFormat.setSamples(8);
//    QSurfaceFormat::setDefaultFormat(defaultFormat);
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    ControlPanel::setQmlEngine(&engine);
    //qmlRegisterType<const ChannelVector*>("my.models",1,0,"ChannelVector");
    qmlRegisterType<ChannelProgrammEditor>("custom.licht",1,0,"ChannelProgrammEditor");
    qmlRegisterType<MapView>("custom.licht",1,0,"MapView");
    qmlRegisterType<MapEditor>("custom.licht",1,0,"MapEditor");
    qmlRegisterType<ControlPanel>("custom.licht",1,0,"ControlPanel");
    qmlRegisterType<ControlItem>("custom.licht.template",1,0,"ControlItemTemplate");
    qmlRegisterType<DimmerGroupControlItemData>("custom.licht",1,0,"DimmerGroupControlItemData");
    qmlRegisterType<DMXChannelFilter>("custom.licht",1,0,"DMXChannelFilter");
    qRegisterMetaType<DMXChannelFilter::Operation>("Operation");
    qmlRegisterType<UserManagment>("custom.licht",1,0,"Permission");
    qRegisterMetaType<UserManagment::Permission>("Permission");
    const auto filename = "QTJSONFile.json";
    QFile file(filename);
    QFile saveFile ("QTJSONFile.json");


#warning Dont use IDBase<xxxxx>::getAllIDBases() in this file. It will crash the aplication when its closing

    auto after = ApplicationData::loadData(file);


    QStringList dataList;
    const QMetaObject &mo = QEasingCurve::staticMetaObject;
    QMetaEnum metaEnum = mo.enumerator(mo.indexOfEnumerator("Type"));
    for (int i = 0; i < QEasingCurve::NCurveTypes - 1; ++i) {
        dataList.append(metaEnum.key(i));
    }

    app.connect(&app,&QGuiApplication::lastWindowClosed,[&](){ApplicationData::saveData(saveFile);});


    engine.rootContext()->setContextProperty("ModelManager",new ModelManager());
    engine.rootContext()->setContextProperty("easingModel",dataList);
    engine.rootContext()->setContextProperty("devicePrototypeModel",IDBaseDataModel<DevicePrototype>::singletone());
    engine.rootContext()->setContextProperty("deviceModel",IDBaseDataModel<Device>::singletone());
    engine.rootContext()->setContextProperty("programmModel",IDBaseDataModel<Programm>::singletone());
    engine.rootContext()->setContextProperty("programmPrototypeModel",IDBaseDataModel<ProgrammPrototype>::singletone());
    engine.rootContext()->setContextProperty("userModel",IDBaseDataModel<User>::singletone());
    engine.rootContext()->setContextProperty("UserManagment",UserManagment::get());

    engine.load(QUrl(QLatin1String("qrc:/main.qml")));

    // laden erst nach dem laden des qml ausführen
    after();
    //ControlPanel::getLastCreated()->addDimmerGroupControl();

    return app.exec();
}
