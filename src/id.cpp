#include "id.h"
#include <QDateTime>
#include <QJsonObject>

ID::value_type ID::lastID = QDateTime::currentMSecsSinceEpoch();

ID::ID(const QJsonObject &o) : id(o[QStringLiteral("id")].toString().toLongLong()) {}

void ID::writeJsonObject(QJsonObject &o) const {
    o.insert(QStringLiteral("id"), QString::number(id));
}
