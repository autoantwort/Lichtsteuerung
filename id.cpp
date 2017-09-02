#include "id.h"

long ID::lastID = QDateTime::currentMSecsSinceEpoch();

ID::ID(const QJsonObject &o):id(o["id"].toString().toLong()){

}

void ID::writeJsonObject(QJsonObject &o) const{
    o.insert("id",QString::number(id));
}


