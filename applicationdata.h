#ifndef APPLICATIONDATA_H
#define APPLICATIONDATA_H

#include <QFile>

namespace ApplicationData{

bool saveData(QFile &file);
bool loadData(QFile &file);
QByteArray saveData();
void loadData(QByteArray data);


}

#endif // APPLICATIONDATA_H
