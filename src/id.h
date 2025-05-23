#ifndef IDGENERATOR_H
#define IDGENERATOR_H

#include <cstdint>

class QJsonObject;
/**
 * @brief Eine ID Klasse, die eindeutige IDs erstellt
 */
class ID {
    friend class UserManagment;

public:
    typedef int64_t value_type;

private:
    static value_type lastID;

public:
    /**
     * @brief generateNew Erstellt eine neue ID
     * @return Eine neue eindeutige ID;
     */
    static ID generateNew() { return ++lastID; }

private:
    const value_type id;
    ID(const value_type id) : id(id) {}

public:
    ID() : id(++lastID) {}
    value_type value() const { return id; }
    bool operator!=(const ID other) const { return other.id != id; }
    bool operator==(const ID other) const { return other.id == id; }
    bool operator!=(const value_type other) const { return other != id; }
    bool operator==(const value_type other) const { return other == id; }
    bool operator<(const ID other) const { return other.id < id; }

public:
    ID(const QJsonObject &o);
    void writeJsonObject(QJsonObject &o) const;
};

#endif // IDGENERATOR_H
