#ifndef MODELVECTOR_H
#define MODELVECTOR_H
#include <vector>
#include <QAbstractListModel>
#include <QDebug>


template<typename Type>
/**
 * @brief The ModelVector class is a wrapper around a std::vector thats support the QAbstractListModel Interface
 */
class ModelVector : public QAbstractListModel{
    /**
     * @brief model The Vector holding the data
     */
    std::vector<Type> model;

    Type get(int index, std::true_type)const{
        return model[index];
    }

    Type* get(int index, std::false_type)const{
        return const_cast<Type*>(&model[index]);
    }

public:
    enum{
        ModelDataRole = Qt::UserRole+1,
    };
    const std::vector<Type>& getVector()const{return model;}
    std::vector<Type>& getVector(){return model;}
    typename std::vector<Type>::size_type size()const{return model.size();}
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
            return QVariant::fromValue(get(index.row(),std::is_pointer<Type>()));
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
    void erase(typename std::vector<Type>::const_iterator i){
        const auto pos = i-model.begin();
        beginRemoveRows(QModelIndex(),pos,pos);
        auto result = *model.erase(i);
        endRemoveRows();
    }
    Type erase(int i){
        Q_ASSERT(i>=0 && i < size());
        beginRemoveRows(QModelIndex(),i,i);
        auto result = model[i];
        model.erase(model.cbegin() + i);
        endRemoveRows();
        return result;
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
    void push_back(Type t){
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
                emit dataChanged(index(0,0),index(model.size()-1,0));
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
                emit dataChanged(index(0,0),index(model.size()-1,0));
            }
        }
    }
    typename std::vector<Type>::reference back(){
        return model.back();
    }
};

#endif // MODELVECTOR_H
