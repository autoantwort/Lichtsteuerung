#include <QCoreApplication>
#include <QDebug>
#include <id.h>
#include <device.h>
#include "channel.h"
#include "applicationdata.h"
#include "programm.h"
int main(int argc, char *argv[])
{

    const auto filename = "QTJSONFile.json";
    QFile file(filename);

    ApplicationData::loadData(file);
    //ProgrammPrototype p(*IDBase<DevicePrototype>::getAllIDBases().begin(), "Test","Mein erstes Programm");
    Programm p("Mein erstes Programm");
    p.addDeviceProgramm(*IDBase<Device>::getAllIDBases().begin(),*IDBase<ProgrammPrototype>::getAllIDBases().begin(),0.5);
    //DevicePrototype c("Scanner");
    //c.addChannel(0,"Speed");
    //c.addChannel(1,"RED");
    //c.addChannel(2,"GREEN");
    //c.addChannel(3,"BLUE");
    new Channel(1);
    new Channel(3);
    delete new Channel(2);
    QCoreApplication a(argc, argv);
    ID id;

    ApplicationData::saveData(file);

    //IDBase<Device>::deleter.hello();
    //IDBase<Channel>::deleter.hello();
    qDebug() << "Hello ich bin cooler \n"<<id.value()<<' '<<IDBase<Device>::getIDBaseObjectByID(0)<<'\n';
    //return a.exec();
    return 0;
}
