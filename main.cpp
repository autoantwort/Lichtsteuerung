#include <QCoreApplication>
#include <QDebug>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qDebug() << "Hello ich bin cooler \n";
    return a.exec();
}
