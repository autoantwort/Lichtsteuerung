#ifndef IDGENERATOR_H
#define IDGENERATOR_H

#include <QDateTime>
#include <QJsonObject>




/**
 * @brief Eine ID Klasse, die eindeutige IDs erstellt
 */
class ID{
    friend class SyncService;
public:
    typedef long long value_type;
private:
    static qint64 lastID;
public:
    /**
     * @brief generateNew Erstellt eine neue ID
     * @return Eine neue eindeutige ID;
     */
    static ID generateNew(){
        return ++lastID;
    }
private:
    const long long id;
    ID(const long long id):id(id){}
public:
    ID():id(++lastID){}
    value_type value()const{return id;}
    bool operator !=(const ID other)const{return other.id!=id;}
    bool operator ==(const ID other)const{return other.id==id;}
    bool operator !=(const value_type other)const{return other!=id;}
    bool operator ==(const value_type other)const{return other==id;}
public:
    ID(const QJsonObject &o);
    void writeJsonObject(QJsonObject &o)const;
};



#endif // IDGENERATOR_H
