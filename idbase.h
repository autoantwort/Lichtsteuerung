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
    template<typename Subclass, typename enableDataModel>
    typename IDBase<Subclass,enableDataModel>::IDBaseSet IDBase<Subclass,enableDataModel>::idBaseObjectsByID;


    template<typename Subclass, typename enableDataModel>
    typename IDBase<Subclass,enableDataModel>::Deleter IDBase<Subclass,enableDataModel>::deleter;





    /**
     *  Ein IDBase data model um alle IDBases einer bestimmten Gruppe zb in einer Liste anzeigen zu lassen.
     *  Aktualisiert sich komplett vollständig
     */
    template<typename IDBaseWithNamedObject,typename first=typename std::is_base_of<IDBase<IDBaseWithNamedObject>,IDBaseWithNamedObject>::type,typename second=typename std::is_base_of<NamedObject,IDBaseWithNamedObject>::type>
    class IDBaseDataModel : public QAbstractListModel{
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
                singletone()->beginRemoveRows(QModelIndex(),pos+1,pos+1);
        }
        static void endRemoveIDBaseObject(){
                singletone()->endRemoveRows();
        }
    public:
        enum NamedObjectRoles{
            NameRole = Qt::UserRole + 1,DescriptionRole
        };
        /**
         * @brief singletone gibt das dataModel zurück
         * @return
         */
        static IDBaseDataModel * singletone(){if(!staticModel){staticModel = new IDBaseDataModel();existDataModel = true;}return staticModel;}
        virtual int rowCount(const QModelIndex &parent = QModelIndex())const override {Q_UNUSED(parent)return IDBase<IDBaseWithNamedObject>::getAllIDBases().size();}
        virtual QVariant data(const QModelIndex &, int role = Qt::DisplayRole)const {
            Q_UNUSED(role)
            return QVariant();
        }
    };


    template<typename IDBaseWithNamedObject,typename f, typename s>
    bool IDBaseDataModel<IDBaseWithNamedObject,f,s>::existDataModel = false;

    template<typename IDBaseWithNamedObject,typename f, typename s>
    IDBaseDataModel<IDBaseWithNamedObject,f,s> * IDBaseDataModel<IDBaseWithNamedObject,f,s>::staticModel = nullptr;




// the model for NamedObjects:

    /**
     *  Ein IDBase data model um alle IDBases einer bestimmten Gruppe zb in einer Liste anzeigen zu lassen.
     *  Aktualisiert sich komplett vollständig
     */
    template<typename IDBaseWithNamedObject>
    class IDBaseDataModel<IDBaseWithNamedObject,std::true_type,std::true_type>: public QAbstractListModel{
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
                singletone()->beginRemoveRows(QModelIndex(),pos+1,pos+1);
        }
        static void endRemoveIDBaseObject(){
                singletone()->endRemoveRows();
        }
    public:
        enum NamedObjectRoles{
            NameRole = Qt::UserRole + 1,DescriptionRole
        };
        /**
         * @brief singletone gibt das dataModel zurück
         * @return
         */
        static IDBaseDataModel * singletone(){if(!staticModel){staticModel = new IDBaseDataModel();existDataModel = true;}return staticModel;}
        virtual int rowCount(const QModelIndex &parent = QModelIndex())const override {Q_UNUSED(parent)return IDBase<IDBaseWithNamedObject>::getAllIDBases().size();}
        virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)const {
            qDebug() << "role"<<role<<" : "<<index.row()<<'\n';
            if(index.row()>=0 && index.row()<static_cast<decltype(index.row())>(IDBase<IDBaseWithNamedObject>::getAllIDBases().size())){
                if(role==Qt::DisplayRole||role==NameRole){
                    return (**std::next(IDBase<IDBaseWithNamedObject>::getAllIDBases().cbegin(),index.row())).getName();
                }else if(role==Qt::ToolTipRole||role==DescriptionRole){
                    return (**std::next(IDBase<IDBaseWithNamedObject>::getAllIDBases().cbegin(),index.row())).getDescription();
                }
            }else{
                qDebug()<<"index out of range\n";
            }
            return QVariant();
        }

        QHash<int,QByteArray> roleNames() const override{
            QHash<int,QByteArray> r;
            r[NameRole] = "name";
            r[DescriptionRole] = "description";
            return r;
        }
    };


    template<typename IDBaseWithNamedObject>
    bool IDBaseDataModel<IDBaseWithNamedObject,std::true_type,std::true_type>::existDataModel = false;

    template<typename IDBaseWithNamedObject>
    IDBaseDataModel<IDBaseWithNamedObject,std::true_type,std::true_type> * IDBaseDataModel<IDBaseWithNamedObject,std::true_type,std::true_type>::staticModel = nullptr;




















    template<typename Subclass,typename enable>
    void IDBase<Subclass,enable>::removeIDBaseObject(IDBase<Subclass,enable> * c)const{
        // checken ob das dataModel enabled ist und ob es exestiert
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
        // checken ob das dataModel enabled ist und ob es exestiert
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
