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
int main(int argc, char *argv[])
{

    const auto filename = "QTJSONFile.json";
    QFile file(filename);
    qDebug()<< QFileInfo(file).absoluteFilePath() <<'\n';

    auto model = IDBaseDataModel<Channel>::singletone();

    ApplicationData::loadData(file);

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);


    qDebug() << model->rowCount()<<'\n';
    for(int i = 0 ; i < model->rowCount();++i){
        qDebug() << model->data(model->index(i))<<'\n';
    }





    app.connect(&app,&QGuiApplication::lastWindowClosed,[&](){ApplicationData::saveData(file);});


    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("deviceModel",IDBaseDataModel<Device>::singletone());
    engine.rootContext()->setContextProperty("devicePrototypeModel",IDBaseDataModel<DevicePrototype>::singletone());
    engine.rootContext()->setContextProperty("programmModel",IDBaseDataModel<Programm>::singletone());
    engine.rootContext()->setContextProperty("programmPrototypeModel",IDBaseDataModel<ProgrammPrototype>::singletone());
    qDebug() << "Number of Elements : "<<IDBaseDataModel<DevicePrototype>::singletone()->rowCount()<<'\n';
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));


    return app.exec();
}
