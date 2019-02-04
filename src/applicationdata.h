#ifndef APPLICATIONDATA_H
#define APPLICATIONDATA_H

#include <QFile>
#include <functional>

namespace ApplicationData{

bool saveData(QFile &file);
std::function<void()> loadData(QFile &file);
QByteArray saveData();
std::function<void()> loadData(QByteArray data);


}

#endif // APPLICATIONDATA_H
