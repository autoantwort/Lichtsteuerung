#ifndef IDBASE_H
#define IDBASE_H

#include "id.h"
#include <QJsonObject>

/**
 *  Eine Klasse die eine Id besitzt
 */
template <typename Subclass>
class IDBase {
    ID id;

public:
    IDBase() = default;
    explicit IDBase(const QJsonObject &o):id(o){}
    void writeJsonObject(QJsonObject &o) const { id.writeJsonObject(o); }

public:
    bool operator==(const IDBase &id) const { return this->id == id.id; }
    bool operator==(ID id) const { return this->id == id; }
    const ID & getID()const{return id;}
};

#endif // IDBASE_H
