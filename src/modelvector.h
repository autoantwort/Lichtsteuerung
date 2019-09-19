#ifndef MODELVECTOR_H
#define MODELVECTOR_H
#include <vector>
#include <QAbstractListModel>
#include <QDebug>
#include <memory>
#include "dmx/namedobject.h"

namespace detail {

template<typename T> //by value
T getPointer(const std::vector<T> &v, int index){
    return v[index];
}

template<typename T> //by pointer
T*  getPointer(const std::vector<T*> &v, int index){
    return v[index];
}

template<typename T> //by std::shared_ptr<T>
T* getPointer(const std::vector<std::shared_ptr<T>> &v, int index){
    return v[index].get();
}

template<typename T> //by std::shared_ptr<T>
T* getPointer(const std::vector<std::unique_ptr<T>> &v, int index){
    return v[index].get();
}

template< class T > struct is_smart_pointer_helper     : std::false_type {};
template< class T > struct is_smart_pointer_helper<std::unique_ptr<T>> : std::true_type {};
template< class T > struct is_smart_pointer_helper<std::shared_ptr<T>> : std::true_type {};
template< class T > struct is_smart_pointer : is_smart_pointer_helper<typename std::remove_cv<T>::type> {};
template< class T > inline constexpr bool is_smart_pointer_v = is_smart_pointer<T>::value;

}

template<typename Type>
/**
 * @brief The ModelVector class is a wrapper around a std::vector thats support the QAbstractListModel Interface
 */
class ModelVector : public QAbstractListModel{
    /**
     * @brief model The Vector holding the data
     */
    std::vector<Type> model;

    /*
      Old code:
        namespace std {
            template<>
              struct __is_pointer_helper<std::shared_ptr<FOO>>
              : public true_type { };
        }


    decltype (&*model.at(std::declval<int>())) get(int index, std::true_type)const{
        // &* to handle smart pointer
        return &*model[index];
    }

    Type* get(int index, std::false_type)const{
        return const_cast<Type*>(&model[index]);
    }*/

public:
    using const_iterator = typename std::vector<Type>::const_iterator;

    enum{
        ModelDataRole = Qt::UserRole+1,
    };
    const std::vector<Type>& getVector()const{return model;}
    std::vector<Type>& getVector(){return model;}
    typename std::vector<Type>::size_type size()const{return model.size();}
    int ssize()const{return static_cast<int>(size());}
    virtual int rowCount(const QModelIndex &) const override{return static_cast<int>(model.size());}
    /**
     * @brief data Return always, if the index is valid, a QVarient that contains the data at the vector
     * @param index the Index in the vector
     * @param role Not used here
     * @return A QVariant which hold a Pointer to the data
     */
    virtual QVariant data(const QModelIndex &index, int role) const override{
        Q_UNUSED(role);
        if(index.row()>=0&&index.row()<int(model.size())){
            //return QVariant::fromValue(get(index.row(),std::is_pointer<Type>()));
            if(role==Qt::DisplayRole){
                if constexpr(std::is_pointer_v<Type>){
                    if constexpr(std::is_base_of_v<DMX::NamedObject,std::remove_pointer<Type>>)
                        return detail::getPointer(model,index.row())->getName();
                    else
                        return "No Display Property available! See ModelVector";
                }else if constexpr(detail::is_smart_pointer_v<Type>){
                    if constexpr(std::is_base_of_v<DMX::NamedObject,typename Type::element_type>)
                        return detail::getPointer(model,index.row())->getName();
                    else
                        return "No Display Property available! See ModelVector";
                }else{
                    if constexpr(std::is_base_of_v<QObject, Type>)
                        return model[index.row()].property("name");
                    else
                        return "No Display Property available! See ModelVector";
                }
            }
            return QVariant::fromValue(detail::getPointer(model,index.row()));
        }
        return QVariant();
    }

    /**
     * @brief roleNames returns the standard role Names and added the Role "modelData"
     * @return
     */
    QHash<int,QByteArray> roleNames()const override{
        QHash<int,QByteArray> r = QAbstractListModel::roleNames();
        r[ModelDataRole] = "modelData";
        return r;
    }
    typename std::vector<Type>::iterator erase(typename std::vector<Type>::const_iterator i){
        const auto pos = std::distance(model.cbegin(),i);
        beginRemoveRows(QModelIndex(),pos,pos);
        const auto result = model.erase(i);
        endRemoveRows();
        return result;
    }
    typename std::vector<Type>::iterator  erase(const_iterator first, const_iterator last){
        const auto pos = std::distance(model.cbegin(),first);
        beginRemoveRows(QModelIndex(),pos,pos + std::distance(first,last) -1);
        const auto result = model.erase(first,last);
        endRemoveRows();
        return result;
    }
    Type erase(int i){
        Q_ASSERT(i>=0 && i < static_cast<int>(size()));
        beginRemoveRows(QModelIndex(),i,i);
        auto result = std::move(model[i]);
        model.erase(model.cbegin() + i);
        endRemoveRows();
        return result;
    }

    template<typename Predicate>
    void remove_if(Predicate p){
        for (auto i = cbegin();i!=cend();) {
            if(p(*i)){
                i = erase(i);
            }else{
                ++i;
            }
        }
    }
    void removeAll(const Type & value){
        remove_if([&](const auto & v){return v == value;});
    }
    /**
     * @brief beginPushBack use this function only if you want to push_back at the underlieingvector by your own, if you are finished, you have to call endPushBack()
     * @param length How much objects you want to push_back
     */
    void beginPushBack(int length){beginInsertRows(QModelIndex(),model.size(),model.size()+length-1);}
    /**
     * @brief endPushBack Call this function if you have started push_backing with beginPushBack and are finisched
     */
    void endPushBack(){endInsertRows();}
    typename std::vector<Type>::const_iterator cbegin()const{
        return model.cbegin();
    }
    typename std::vector<Type>::const_iterator cend()const{
        return model.cend();
    }
    typename std::vector<Type>::const_iterator begin()const{
        return model.begin();
    }
    typename std::vector<Type>::const_iterator end()const{
        return model.end();
    }
    typename std::vector<Type>::iterator begin(){
        return model.begin();
    }
    typename std::vector<Type>::iterator end(){
        return model.end();
    }
    /**
     * @brief push_back Adds one Element at the back
     * @param t The Element to add
     */
    void push_back(const Type& t){
        if(std::is_pointer<Type>()){
            beginPushBack(1);
            model.push_back(t);
            endPushBack();
        }else{
            auto cap = model.capacity();
            beginPushBack(1);
            model.push_back(t);
            endPushBack();
            if(cap!=model.capacity()){
                emit QAbstractItemModel::dataChanged(index(0,0),index(model.size()-1,0));
            }
        }
    }
    /**
     * @brief push_back Adds one Element at the back
     * @param t The Element to add
     */
    void push_back(Type&& t){
        if(std::is_pointer<Type>()){
            beginPushBack(1);
            model.push_back(std::forward<Type>(t));
            endPushBack();
        }else{
            auto cap = model.capacity();
            beginPushBack(1);
            model.push_back(std::forward<Type>(t));
            endPushBack();
            if(cap!=model.capacity()){
                emit QAbstractItemModel::dataChanged(index(0,0),index(model.size()-1,0));
            }
        }
    }
    template<typename... _Args>
    void
    emplace_back(_Args&&... __args){
        if(std::is_pointer<Type>()){
        beginPushBack(1);
        model.emplace_back(std::forward<_Args>(__args)...);
        endPushBack();
        }else{
            auto cap = model.capacity();
            beginPushBack(1);
            model.emplace_back(std::forward<_Args>(__args)...);
            endPushBack();
            if(cap!=model.capacity()){
                emit QAbstractItemModel::dataChanged(index(0,0),index(model.size()-1,0));
            }
        }
    }
    typename std::vector<Type>::reference back(){
        return model.back();
    }

    typename std::vector<Type>::reference operator[](int index){
        return model[index];
    }

    void dataChanged(int index_){
        emit QAbstractItemModel::dataChanged(index(index_), index(index_));
    }
    void dataChanged(int index_, int length){
        emit QAbstractItemModel::dataChanged(index(index_), index(index_ + length - 1));
    }

    typename std::vector<Type>::const_reference operator[](int index)const{
        return model[index];
    }

    void resize(typename std::vector<Type>::size_type size){
        auto diff = static_cast<int>(size) - ssize();
        if(diff == 0){
            return;
        }
        if(diff > 0) {
            beginPushBack(diff);
        } else {
            beginRemoveRows(QModelIndex(),ssize()+diff, ssize()-1);
        }
        model.resize(size);
        if(diff > 0) {
            endPushBack();
        } else {
            endRemoveRows();
        }
    }

    void clear(){
        if(size()==0){
            return;
        }
        beginRemoveRows(QModelIndex(),0,size()-1);
        model.clear();
        endRemoveRows();
    }
};

#endif // MODELVECTOR_H
