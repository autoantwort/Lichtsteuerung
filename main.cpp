#include <QCoreApplication>
#include <QDebug>
#include <id.h>
#include <device.h>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ID id;


    qDebug() << "Hello ich bin cooler \n"<<id.value()<<' '<<IDBase<Device>::getIDBaseObjectByID(0)<<'\n';
    return a.exec();
}
