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
int main(int argc, char *argv[])
{

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    const auto filename = "QTJSONFile.json";
    QFile file(filename);

    auto model = IDBaseDataModel<Channel>::singletone();

    ApplicationData::loadData(file);



    qDebug() << model->rowCount()<<'\n';
    for(int i = 0 ; i < model->rowCount();++i){
        qDebug() << model->data(model->index(i))<<'\n';
    }



    ApplicationData::saveData(file);




    QQmlApplicationEngine engine;
    engine.load(QUrl(QLatin1String("qrc:/Test.qml")));


    return app.exec();
}
