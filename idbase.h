#ifndef IDBASE_H
#define IDBASE_H
#include <set>
#include <assert.h>
#include <QDebug>
#include "id.h"
#include <type_traits>
#include <QJsonObject>
#include <iostream>
#include <QAbstractListModel>
#include "namedobject.h"

template<typename Subclass>
class IDBase;
namespace detail{
template<typename Subclass>
struct IDBaseComperator{
    typedef std::true_type is_transparent;
    bool operator()(const IDBase<Subclass> *l,const IDBase<Subclass> *r);
    bool operator()(const long l,const IDBase<Subclass> *r);
    bool operator()(const IDBase<Subclass> *l,const long r);
};

template<typename Subclass>
inline typename std::enable_if<std::is_base_of<QObject,Subclass>::value, void>::type addIDBaseObject(std::set<Subclass*,detail::IDBaseComperator<Subclass>> &set, IDBase<Subclass> * value);
template<typename Subclass>
inline typename std::enable_if<!std::is_base_of<QObject,Subclass>::value, void>::type addIDBaseObject(std::set<Subclass*,detail::IDBaseComperator<Subclass>> &set, IDBase<Subclass> * value);

template<typename Subclass>
inline typename std::enable_if<std::is_base_of<QObject,Subclass>::value, void>::type removeIDBaseObject(std::set<Subclass*,detail::IDBaseComperator<Subclass>> &set, IDBase<Subclass> * value);
template<typename Subclass>
inline typename std::enable_if<!std::is_base_of<QObject,Subclass>::value, void>::type removeIDBaseObject(std::set<Subclass*,detail::IDBaseComperator<Subclass>> &set, IDBase<Subclass> * value);

}
/**
     * Eine Basis Klasse für Klassen, die eine Eindeutige ID benitzen sollen und bei denen mit der ID nach Objecten gesucht werden kann.
     * Objekte die auf dem Heap erzeugt werden, werden automatisch gelöscht, sollten sie nicht vom Programmierer gelöscht werden.
     * Subclass ist der größte gemeinsame Nenner, in den die Klasse eingeteilt werden soll.
     */
template<typename Subclass>
class IDBase{
    ID id;
    /**
         * @brief The Deleter class deletes all Objects that are on the heap when the deleter gets destroyed
         */
    class Deleter{
    public:
        ~Deleter(){
            //qDebug() << IDBase<Subclass>::getAllIDBases().size() << " Objects have to be deleted from Type "<< typeid(Subclass).name() <<"\n";
            while (!IDBase<Subclass>::getAllIDBases().empty()) {
                delete *IDBase<Subclass>::getAllIDBases().crbegin();
            }
        }
        // Wir brauchen ein Dummy funktion, um so zu tun, als würden wir das object benutzten, da es sonst nicht erzeugt wird
        void dummy(){}
    };
    static Deleter deleter;
public:
    typedef std::set<Subclass*,detail::IDBaseComperator<Subclass>> IDBaseSet;
    IDBase(){
        // wir müssen so tun, als würden wir das object benutzten, da es sonst nicht erzeugt wird.
        deleter.dummy();
        assert(getIDBaseObjectByID(id)==nullptr);
        addIDBaseObject(this);
    }
    IDBase(const QJsonObject &o):id(o){
        // wir müssen so tun, als würden wir das object benutzten, da es sonst nicht erzeugt wird.
        deleter.dummy();
        assert(getIDBaseObjectByID(id)==nullptr);
        addIDBaseObject(this);
    }
    void writeJsonObject(QJsonObject &o)const{id.writeJsonObject(o);}
    virtual ~IDBase(){
        //qDebug()<<"Call Destructor from : "<<getID().value()<<'\n';
        removeIDBaseObject(this);
    }
private:
    static IDBaseSet idBaseObjectsByID;
    void removeIDBaseObject(IDBase * c)const{
        detail::removeIDBaseObject(idBaseObjectsByID,c);
    }
    void addIDBaseObject(IDBase * c)const{
        detail::addIDBaseObject(idBaseObjectsByID,c);
    }


public:
    /*
        static void deleteIDBase(int id){
            auto found = getIDBaseByID(id);
            if (found!=nullptr) {
                delete found;
            }
        }*/

    bool operator ==(const IDBase &id)const{return this->id==id;}
    //bool operator ==(const int id)const{return this->id.value()==id;}

    /**
         * @brief getIDBaseObjectByID Get an Object by its id
         * @param id The Id to search for
         * @return A Pointer to the Object with the ID, or a nullptr. Maybe an Pointer to the Stack, be careful.
         */
    static Subclass * getIDBaseObjectByID(const ID &id){
        return getIDBaseObjectByID(id.value());
    }

    static Subclass * getIDBaseObjectByID(const QJsonValue &o){
        return getIDBaseObjectByID(o.toString().toLong());
    }
    /**
         * @brief getIDBaseObjectByID Get an Object by its id
         * @param id The Id to search for
         * @return A Pointer to the Object with the ID, or a nullptr. Maybe an Pointer to the Stack, be careful.
         */
    static Subclass * getIDBaseObjectByID(long id){
        static_assert(std::is_base_of<IDBase<Subclass>,Subclass>::value,"The Subclass Template Parameter Type is not a Subclass of IDBase");
        auto found = idBaseObjectsByID.find(id);
        if (found!=idBaseObjectsByID.cend()) {
            return static_cast<Subclass*>(*found);
        }
        //qDebug() << "Dont find IDBase Object with id "<<id << "and type "<<typeid(Subclass).name()<<'\n';
        return nullptr;
    }
    static const IDBaseSet& getAllIDBases(){return idBaseObjectsByID;}
    const ID & getID()const{return id;}
};
namespace detail{
template<typename Subclass>
bool IDBaseComperator<Subclass>::operator()(const IDBase<Subclass> *l,const IDBase<Subclass> *r){
    return l->getID().value()<r->getID().value();
}

template<typename Subclass>
bool IDBaseComperator<Subclass>::operator()(const long l,const IDBase<Subclass> *r){
    return l<r->getID().value();
}

template<typename Subclass>
bool IDBaseComperator<Subclass>::operator()(const IDBase<Subclass> *l,const long r){
    return l->getID().value()<r;
}

}
template<typename Subclass>
typename IDBase<Subclass>::IDBaseSet IDBase<Subclass>::idBaseObjectsByID;


template<typename Subclass>
typename IDBase<Subclass>::Deleter IDBase<Subclass>::deleter;





/**
     *  Ein IDBase data model um alle IDBases einer bestimmten Gruppe zb in einer Liste anzeigen zu lassen.
     *  Aktualisiert sich komplett vollständig
     */
template<typename IDBaseWithNamedObject>
class IDBaseDataModel : public QAbstractListModel{

    template<typename Subclass>
    friend inline typename std::enable_if<std::is_base_of<QObject,Subclass>::value, void>::type detail::addIDBaseObject(std::set<Subclass*,detail::IDBaseComperator<Subclass>> &set, IDBase<Subclass> * value);

    template<typename Subclass>
    friend typename std::enable_if<std::is_base_of<QObject,Subclass>::value, void>::type detail::removeIDBaseObject(std::set<Subclass*,detail::IDBaseComperator<Subclass>> &set, IDBase<Subclass> * value);
private:
    IDBaseDataModel(){}
    static IDBaseDataModel * staticModel;
    static bool existDataModel;
    friend class IDBase<IDBaseWithNamedObject>;
    static void beginAddIDBaseObject(typename IDBase<IDBaseWithNamedObject>::IDBaseSet::const_iterator c){
        const auto pos = std::distance(IDBase<IDBaseWithNamedObject>::getAllIDBases().cbegin(),c);
        singletone()->beginInsertRows(QModelIndex(),pos,pos);
    }
    static void endAddIDBaseObject(){
        singletone()->endInsertRows();
    }
    static void beginRemoveIDBaseObject(typename IDBase<IDBaseWithNamedObject>::IDBaseSet::const_iterator c){
        const auto pos = std::distance(IDBase<IDBaseWithNamedObject>::getAllIDBases().cbegin(),c);
        singletone()->beginRemoveRows(QModelIndex(),pos,pos);
    }
    static void endRemoveIDBaseObject(){
        singletone()->endRemoveRows();
    }
public:
    enum NamedObjectRoles{
        ItemDataRole = Qt::UserRole + 1
    };
    /**
         * @brief singletone gibt das dataModel zurück
         * @return
         */
    static IDBaseDataModel * singletone(){if(!staticModel){staticModel = new IDBaseDataModel();existDataModel = true;}return staticModel;}
    virtual int rowCount(const QModelIndex &parent = QModelIndex())const override {Q_UNUSED(parent)return IDBase<IDBaseWithNamedObject>::getAllIDBases().size();}
    IDBaseWithNamedObject * data(int row)const {
        if(row>=0 && row<static_cast<int>(IDBase<IDBaseWithNamedObject>::getAllIDBases().size())){
            return *std::next(IDBase<IDBaseWithNamedObject>::getAllIDBases().cbegin(),row);
        }
        return nullptr;
    }
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)const override{
        if(index.row()>=0 && index.row()<static_cast<decltype(index.row())>(IDBase<IDBaseWithNamedObject>::getAllIDBases().size())){
            IDBaseWithNamedObject * pointer = *std::next(IDBase<IDBaseWithNamedObject>::getAllIDBases().cbegin(),index.row());
            //qDebug()<<pointer->metaObject()->className()<<role<<'\n';
            switch(role){
            case Qt::ToolTipRole:
            case Qt::DisplayRole:
              //  qDebug()<<"Value"<<pointer->property("name");
                return pointer->property("name");
            }
            //qDebug()<<"return pointer";
            return QVariant::fromValue(pointer);
        }else{
            qDebug()<<"index out of range row x colum : "<<index.row()<<' '<<index.column();
        }
        return QVariant("Index Out of Range");
    }
    QHash<int,QByteArray> roleNames() const override{
        QHash<int,QByteArray> r;
        r[ItemDataRole] = "itemData";
        r[Qt::DisplayRole] = "display";
        r[Qt::ToolTipRole] = "tooltip";
        return r;
    }
};

template<typename IDBaseWithNamedObject>
bool IDBaseDataModel<IDBaseWithNamedObject>::existDataModel = false;

template<typename IDBaseWithNamedObject>
IDBaseDataModel<IDBaseWithNamedObject> * IDBaseDataModel<IDBaseWithNamedObject>::staticModel = nullptr;


namespace detail {



template<typename Subclass>
inline typename std::enable_if<std::is_base_of<QObject,Subclass>::value, void>::type addIDBaseObject(std::set<Subclass*,detail::IDBaseComperator<Subclass>> &set, IDBase<Subclass> * value){
    const auto iter = set.lower_bound(static_cast<Subclass*>(value));
    IDBaseDataModel<Subclass>::beginAddIDBaseObject(iter);
    set.insert(iter,static_cast<Subclass*>(value));
    IDBaseDataModel<Subclass>::endAddIDBaseObject();
}

template<typename Subclass>
inline typename std::enable_if<!std::is_base_of<QObject,Subclass>::value, void>::type addIDBaseObject(std::set<Subclass*,detail::IDBaseComperator<Subclass>> &set, IDBase<Subclass> * value){
    set.insert(static_cast<Subclass*>(value));
}

template<typename Subclass>
inline typename std::enable_if<std::is_base_of<QObject,Subclass>::value, void>::type removeIDBaseObject(std::set<Subclass*,detail::IDBaseComperator<Subclass>> &set, IDBase<Subclass> * value){
    const auto iter = set.find(static_cast<Subclass*>(value));
    IDBaseDataModel<Subclass>::beginRemoveIDBaseObject(iter);
    set.erase(iter);
    IDBaseDataModel<Subclass>::endRemoveIDBaseObject();
}

template<typename Subclass>
inline typename std::enable_if<!std::is_base_of<QObject,Subclass>::value, void>::type removeIDBaseObject(std::set<Subclass*,detail::IDBaseComperator<Subclass>> &set, IDBase<Subclass> * value){
    set.erase(static_cast<Subclass*>(value));
}

}






#endif // IDBASE_H
