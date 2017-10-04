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

    template<typename Subclass, typename enableDataModel = std::true_type>
    class IDBase;
    namespace detail{
        template<typename Subclass>
        struct IDBaseComperator{
            typedef std::true_type is_transparent;
            bool operator()(const IDBase<Subclass> *l,const IDBase<Subclass> *r);
            bool operator()(const long l,const IDBase<Subclass> *r);
            bool operator()(const IDBase<Subclass> *l,const long r);
        };
    }
    /**
     * Eine Basis Klasse für Klassen, die eine Eindeutige ID benitzen sollen und bei denen mit der ID nach Objecten gesucht werden kann.
     * Objekte die auf dem Heap erzeugt werden, werden automatisch gelöscht, sollten sie nicht vom Programmierer gelöscht werden.
     * Subclass ist der größte gemeinsame Nenner, in den die Klasse eingeteilt werden soll.
     */
    template<typename Subclass, typename enableDataModel>
    class IDBase{
        ID id;
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
            qDebug()<<"Call Destructor from : "<<getID().value()<<'\n';
            removeIDBaseObject(this);
        }
    private:
        static IDBaseSet idBaseObjectsByID;
        void removeIDBaseObject(IDBase * c)const;/*{
            idBaseObjectsByID.erase(static_cast<Subclass*>(c));
        }*/
        void addIDBaseObject(IDBase * c)const;/*{
            idBaseObjectsByID.insert(static_cast<Subclass*>(c));
        }*/
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
            static_assert(std::is_base_of<IDBase<Subclass,enableDataModel>,Subclass>::value,"The Subclass Template Parameter Type is not a Subclass of IDBase");
            auto found = idBaseObjectsByID.find(id);
            if (found!=idBaseObjectsByID.cend()) {
                return static_cast<Subclass*>(*found);
            }
            qDebug() << "Dont find IDBase Object with id "<<id << "and type "<<typeid(Subclass).name()<<'\n';
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
    template<typename Subclass, typename enableDataModel>
    typename IDBase<Subclass,enableDataModel>::IDBaseSet IDBase<Subclass,enableDataModel>::idBaseObjectsByID;


    template<typename Subclass, typename enableDataModel>
    typename IDBase<Subclass,enableDataModel>::Deleter IDBase<Subclass,enableDataModel>::deleter;


    template<typename IDBaseWithNamedObject>
    class IDBaseDataModel : public QAbstractListModel{
    private:
        IDBaseDataModel(){}
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
                singletone()->beginRemoveRows(QModelIndex(),pos+1,pos+1);
        }
        static void endRemoveIDBaseObject(){
                singletone()->endRemoveRows();
        }
        QVariant dataCheck(const QModelIndex &index, int role, std::true_type,std::true_type)const{
            if(index.row()>=0 && index.row()<static_cast<decltype(index.row())>(IDBase<IDBaseWithNamedObject>::getAllIDBases().size())){
                if(role==Qt::DisplayRole){
                    return (**std::next(IDBase<IDBaseWithNamedObject>::getAllIDBases().cbegin(),index.row())).getName();
                }else if(role==Qt::ToolTipRole){
                    return (**std::next(IDBase<IDBaseWithNamedObject>::getAllIDBases().cbegin(),index.row())).getDescription();
                }
            }
            return QVariant();
        }
        template<typename T, typename U>
        QVariant dataCheck(const QModelIndex &, int, T,U)const{
            return QVariant();
        }
    public:
        static IDBaseDataModel * singletone(){static IDBaseDataModel s;existDataModel = true;return &s;}
        virtual int rowCount(const QModelIndex &parent = QModelIndex())const override {Q_UNUSED(parent)return IDBase<IDBaseWithNamedObject>::getAllIDBases().size();}
        virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)const {
            return dataCheck(index,role,std::is_base_of<IDBase<IDBaseWithNamedObject>,IDBaseWithNamedObject>::value , std::is_base_of<NamedObject,IDBaseWithNamedObject>::value);
        }
    };


    template<typename IDBaseWithNamedObject>
    bool IDBaseDataModel<IDBaseWithNamedObject>::existDataModel = false;



    template<typename Subclass,typename enable>
    void IDBase<Subclass,enable>::removeIDBaseObject(IDBase<Subclass,enable> * c)const{
        if(std::is_same<enable,std::true_type>::value && IDBaseDataModel<Subclass>::existDataModel){
            const auto iter = idBaseObjectsByID.find(static_cast<Subclass*>(c));
            IDBaseDataModel<Subclass>::beginRemoveIDBaseObject(iter);
            idBaseObjectsByID.erase(iter);
            IDBaseDataModel<Subclass>::endRemoveIDBaseObject();
        }else{
            idBaseObjectsByID.erase(static_cast<Subclass*>(c));
        }
    }

    template<typename Subclass,typename enable>
    void IDBase<Subclass,enable>::addIDBaseObject(IDBase<Subclass,enable> * c)const{
        if(std::is_same<enable,std::true_type>::value && IDBaseDataModel<Subclass>::existDataModel){
            const auto iter = idBaseObjectsByID.lower_bound(static_cast<Subclass*>(c));
            IDBaseDataModel<Subclass>::beginAddIDBaseObject(iter);
            idBaseObjectsByID.insert(iter,static_cast<Subclass*>(c));
            IDBaseDataModel<Subclass>::endAddIDBaseObject();
        }else{
            idBaseObjectsByID.insert(static_cast<Subclass*>(c));
        }
    }

#endif // IDBASE_H
