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
int main(int argc, char *argv[])
{

    //qmlRegisterType<const ChannelVector*>("my.models",1,0,"ChannelVector");
    qmlRegisterType<ChannelProgrammEditor>("custom.licht",1,0,"ChannelProgrammEditor");

    const auto filename = "QTJSONFile.json";
    QFile file(filename);
    qDebug()<< QFileInfo(file).absoluteFilePath() <<'\n';

    auto model = IDBaseDataModel<Channel>::singletone();

    ApplicationData::loadData(file);

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    qDebug()<<std::numeric_limits<long>::max()<<'\n';
    qDebug()<<std::numeric_limits<long long>::max()<<'\n';
        qDebug()<<std::numeric_limits<qint64>::max()<<'\n';

    qDebug() << model->rowCount()<<'\n';
    for(int i = 0 ; i < model->rowCount();++i){
        qDebug() << model->data(model->index(i))<<'\n';
    }

    QStringList dataList;
    const QMetaObject &mo = QEasingCurve::staticMetaObject;
    QMetaEnum metaEnum = mo.enumerator(mo.indexOfEnumerator("Type"));
    for (int i = 0; i < QEasingCurve::NCurveTypes - 1; ++i) {
        dataList.append(metaEnum.key(i));
    }

    app.connect(&app,&QGuiApplication::lastWindowClosed,[&](){ApplicationData::saveData(file);});


    /*QSurfaceFormat format = view.format();
    format.setSamples(16);
    view.setFormat(format);*/
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("ModelManager",new ModelManager());
    engine.rootContext()->setContextProperty("deviceModel",IDBaseDataModel<Device>::singletone());
    engine.rootContext()->setContextProperty("easingModel",dataList);
    engine.rootContext()->setContextProperty("devicePrototypeModel",IDBaseDataModel<DevicePrototype>::singletone());
    engine.rootContext()->setContextProperty("programmModel",IDBaseDataModel<Programm>::singletone());
    engine.rootContext()->setContextProperty("programmPrototypeModel",IDBaseDataModel<ProgrammPrototype>::singletone());
    qDebug() << "Number of Elements : "<<IDBaseDataModel<DevicePrototype>::singletone()->rowCount()<<'\n';
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));


    return app.exec();
}
