#ifndef IDBASE_H
#define IDBASE_H
#include <set>
#include <assert.h>
#include <QDebug>
#include "id.h"
#include <type_traits>

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
    }
    /**
     * Eine Basis Klasse für Klassen, die eine Eindeutige ID benitzen sollen und bei denen mit der ID nach Objecten gesucht werden kann.
     * Subclass ist der größte gemeinsame Nenner, in den die Klasse eingeteilt werden soll.
     */
    template<typename Subclass>
    class IDBase{
        ID id;
    public:
        typedef std::set<Subclass*,detail::IDBaseComperator<Subclass>> IDBaseSet;
        IDBase(){
            assert(getIDBaseObjectByID(id)==nullptr);
            addIDBaseObject(this);
        }
        IDBase(const QJsonObject &o):id(o){
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
        void removeIDBaseObject(IDBase * c)const{
            idBaseObjectsByID.erase(idBaseObjectsByID.find(static_cast<Subclass*>(c)));
        }
        void addIDBaseObject(IDBase * c)const{
            idBaseObjectsByID.insert(static_cast<Subclass*>(c));
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
    template<typename Subclass>
    typename IDBase<Subclass>::IDBaseSet IDBase<Subclass>::idBaseObjectsByID;

#endif // IDBASE_H
