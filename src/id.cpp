#include "id.h"

ID::value_type ID::lastID = QDateTime::currentMSecsSinceEpoch();

ID::ID(const QJsonObject &o) : id(o["id"].toString().toLongLong()) {}

void ID::writeJsonObject(QJsonObject &o) const {
    o.insert("id", QString::number(id));
}
