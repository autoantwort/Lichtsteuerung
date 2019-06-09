#ifndef SORTEDMODELVIEW_H
#define SORTEDMODELVIEW_H

#include <QSortFilterProxyModel>
#include <modelvector.h>
#include <idbase.h>

/**
 * @brief The SortedModelVectorView class has the ability the sort ModelVectors by providing the name of the property based on which should be sorted
 */
class SortedModelVectorView : public QSortFilterProxyModel
{
    Q_OBJECT
    std::vector<QString> sortPropertyName;
    std::vector<std::string> sortPropertyNameAsStdStrings;
    Qt::SortOrder sortOrder = Qt::AscendingOrder;
    bool sortCalled = false;
    Q_PROPERTY(QString sortPropertyName READ getSortPropertyName WRITE setSortPropertyName NOTIFY sortPropertyNameChanged)
    Q_PROPERTY(Qt::SortOrder sortOrder READ getSortOrder WRITE setSortOrder NOTIFY sortOrderChanged)
public:
    SortedModelVectorView(QObject *parent = nullptr);
    template<typename T>
    SortedModelVectorView(ModelVector<T> * modelVector):QSortFilterProxyModel (modelVector){
        QSortFilterProxyModel::setSourceModel(modelVector);
        setSortRole(ModelVector<T>::ModelDataRole);
    }
    /**
     * @brief setSouceModel sets the source model, the parent object of this class is set to the source model.
     * @param modelVector the ModelVector which should be the source model
     */
    template<typename T>
    void setSouceModel(const ModelVector<T> & modelVector){
        setParent(&modelVector);
        QSortFilterProxyModel::setSourceModel(&modelVector);
        setSortRole(ModelVector<T>::ModelDataRole);
    }
    /**
     * @brief setSortPropertyName sets the name of the property based on which should be sorted. Can be an sub-property too (an property of an object, which is
     * an property of the object stored in this model), eg. "startDMXChannel or "prototype.numberOfChannels"
     * @param sortPropertyName The name ob the property
     */
    void setSortPropertyName(const QString& sortPropertyName);
    /**
     * @brief getSortPropertyName returns the name of the property based on which should be sorted
     * @return
     */
    QString getSortPropertyName()const;
    void setSortOrder(Qt::SortOrder order);
    Qt::SortOrder getSortOrder()const{return sortOrder;}
    // we override this function in order to check if we are in a state where we can sort the model
    virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;
    void sort(){sort(0,sortOrder);}
signals:
    void sortPropertyNameChanged();
    void sortOrderChanged();
protected:
    virtual bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;
private:
    QString getSortPropertyNameAsString()const;
};

#endif // SORTEDMODELVIEW_H
