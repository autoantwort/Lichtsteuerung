#ifndef MODELVECTOR_H
#define MODELVECTOR_H
#include <vector>
#include <QAbstractListModel>
#include <QDebug>

template<typename Type>
class ModelVector : public QAbstractListModel{
    std::vector<Type*> model;
public:
    enum{
        ModelDataRole = Qt::UserRole+1,
    };
    const std::vector<Type*>& getVector()const{return model;}
    std::vector<Type*>& getVector(){return model;}
    typename std::vector<Type*>::size_type size()const{return model.size();}
    virtual int rowCount(const QModelIndex &) const override{return model.size();}
    virtual QVariant data(const QModelIndex &index, int role) const override{
        Q_UNUSED(role);
        if(index.row()>=0&&index.row()<int(model.size())){
            return QVariant::fromValue(model[index.row()]);
        }
        return QVariant();
    }
    QHash<int,QByteArray> roleNames()const override{
        QHash<int,QByteArray> r;
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
    void beginPushBack(int length){beginInsertRows(QModelIndex(),model.size(),model.size()+length-1);}
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
