#include <QCoreApplication>
#include <QDebug>
#include <id.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ID id;

    qDebug() << "Hello ich bin cooler \n"<<id.value();
    return a.exec();
}
