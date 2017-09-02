#ifndef IDGENERATOR_H
#define IDGENERATOR_H

#include <QDateTime>
#include <QJsonObject>




/**
 * @brief Eine ID Klasse, die eindeutige IDs erstellt
 */
class ID{
private:
    static long lastID;
public:
    /**
     * @brief generateNew Erstellt eine neue ID
     * @return Eine neue eindeutige ID;
     */
    static ID generateNew(){
        return ++lastID;
    }
private:
    const long id;
    ID(const long id):id(id){}
public:
    ID():id(++lastID){}
    long value()const{return id;}
    bool operator !=(const ID other)const{return other.id!=id;}
    bool operator ==(const ID other)const{return other.id!=id;}
public:
    ID(const QJsonObject &o);
    void writeJsonObject(QJsonObject &o)const;
};



#endif // IDGENERATOR_H
