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
    std::vector<Type*> model;
public:
    enum{
        ModelDataRole = Qt::UserRole+1,
    };
    const std::vector<Type*>& getVector()const{return model;}
    std::vector<Type*>& getVector(){return model;}
    typename std::vector<Type*>::size_type size()const{return model.size();}
    virtual int rowCount(const QModelIndex &) const override{return model.size();}
    /**
     * @brief data Return always, if the index is valid, a QVarient that contains the data at the vector
     * @param index the Index in the vector
     * @param role Not used here
     * @return A QVariant which hold a Pointer to the data
     */
    virtual QVariant data(const QModelIndex &index, int role) const override{
        Q_UNUSED(role);
        if(index.row()>=0&&index.row()<int(model.size())){
            return QVariant::fromValue(model[index.row()]);
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
    Type* erase(typename std::vector<Type*>::const_iterator i){
        const auto pos = i-model.begin();
        beginRemoveRows(QModelIndex(),pos,pos);
        auto result = *model.erase(i);
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
    typename std::vector<Type*>::const_iterator cbegin()const{
        return model.cbegin();
    }
    typename std::vector<Type*>::const_iterator cend()const{
        return model.cend();
    }
    typename std::vector<Type*>::iterator begin(){
        return model.begin();
    }
    typename std::vector<Type*>::iterator end(){
        return model.end();
    }
    /**
     * @brief push_back Adds one Element at the back
     * @param t The Element to add
     */
    void push_back(Type * t){
        beginPushBack(1);
        model.push_back(t);
        endPushBack();
    }
    typename std::vector<Type*>::reference back(){
        return model.back();
    }
};

#endif // MODELVECTOR_H
