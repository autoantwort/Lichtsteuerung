#include "sortedmodelview.h"

SortedModelVectorView::SortedModelVectorView(QObject *parent):QSortFilterProxyModel (parent)
{

}

void SortedModelVectorView::setSortPropertyName(const QString &sortPropertyName){
    this->sortPropertyName.clear();
    sortPropertyNameAsStdStrings.clear();
    for(const auto & s : sortPropertyName.split(".")){
        this->sortPropertyName.push_back(s);
        sortPropertyNameAsStdStrings.push_back(s.toStdString());
    }
    sort(0);
}

QString SortedModelVectorView::getSortPropertyName() const{
    QString res;
    for(const auto & s : sortPropertyName){
        res += s;
    }
    return res;
}

inline QVariant getValue(QObject * start, const std::vector<std::string> names){
    for (auto i = names.cbegin(); i != names.cend() - 1 && start != nullptr; ++i) {
        start = start->property(i->c_str()).value<QObject*>();
    }
    if(start){
        return start->property(names.back().c_str());
    }
    return QVariant();
}

void SortedModelVectorView::sort(int column, Qt::SortOrder order){
    //check if we can sort
    if(sortPropertyName.empty()){
        qWarning("For a SortedModelVectorView no sortPropertyName is set!");
        return;
    }
    if(rowCount()<1)
        return;
    QVariant value = data(index(0,0),sortRole());
    if(!value.isValid()){
        qWarning("For a SortedModelVectorView no valied data are availible for the current sortRole!");
        return;
    }
    if(auto p = value.value<QObject*>(); p != nullptr){
        if(getValue(p,sortPropertyNameAsStdStrings).isValid()){
            QSortFilterProxyModel::sort(column,order);
        }else{
            qWarning("For a SortedModelVectorView the sortPropertyName does not exists!");
        }
    }else{
        qWarning("For a SortedModelVectorView the data for the current sortRole is not a QObject* !");
    }
}

bool SortedModelVectorView::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const{
    const auto left = getValue(source_left.data(sortRole()).value<QObject*>(),sortPropertyNameAsStdStrings);
    const auto right = getValue(source_right.data(sortRole()).value<QObject*>(),sortPropertyNameAsStdStrings);
    return left < right;
}
