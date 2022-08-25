#include "sortedmodelview.h"
#include <dmx/device.h>
SortedModelVectorView::SortedModelVectorView(QObject *parent) : QSortFilterProxyModel(parent) {
    // Used by Modelvector
    setSortRole(Qt::UserRole + 1);
}

void SortedModelVectorView::setSortPropertyName(const QString &sortPropertyName) {
    this->sortPropertyName.clear();
    sortPropertyNameAsStdStrings.clear();
    for (const auto &s : sortPropertyName.split(".")) {
        if (s.isEmpty()) continue;
        this->sortPropertyName.push_back(s);
        sortPropertyNameAsStdStrings.push_back(s.toStdString());
    }
    invalidate();
    if (!sortCalled) {
        sort();
    }
}

QString SortedModelVectorView::getSortPropertyName() const {
    QString res;
    for (const auto &s : sortPropertyName) {
        res += s;
    }
    return res;
}

inline QVariant getValue(QObject *start, const std::vector<std::string> names) {
    for (auto i = names.cbegin(); i != names.cend() - 1 && start != nullptr; ++i) {
        start = start->property(i->c_str()).value<QObject *>();
    }
    if (start) {
        return start->property(names.back().c_str());
    }
    return QVariant();
}

void SortedModelVectorView::setSortOrder(Qt::SortOrder order) {
    if (order != sortOrder) {
        sortOrder = order;
        emit sortOrderChanged();
        sort();
    }
}

void SortedModelVectorView::sort(int column, Qt::SortOrder order) {
    // check if we can sort
    if (sortPropertyNameAsStdStrings.empty()) {
        QSortFilterProxyModel::sort(column, order);
        sortCalled = true;
        return;
    }
    if (rowCount() < 1) return;
    QVariant value = data(index(0, 0), sortRole());
    if (!value.isValid()) {
        qWarning("For a SortedModelVectorView no valied data are availible for the current sortRole!");
        return;
    }
    if (auto p = value.value<QObject *>(); p != nullptr) {
        if (getValue(p, sortPropertyNameAsStdStrings).isValid()) {
            QSortFilterProxyModel::sort(column, order);
            sortCalled = true;
        } else {
            qWarning() << "For a SortedModelVectorView the sortPropertyName does not exists : " << getSortPropertyNameAsString();
        }
    } else {
        qWarning("For a SortedModelVectorView the data for the current sortRole is not a QObject* !");
    }
}

bool SortedModelVectorView::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const {
    if (sortPropertyNameAsStdStrings.empty()) {
        return source_left.row() < source_right.row();
    }
    const auto left = getValue(source_left.data(sortRole()).value<QObject *>(), sortPropertyNameAsStdStrings);
    const auto right = getValue(source_right.data(sortRole()).value<QObject *>(), sortPropertyNameAsStdStrings);
    return QPartialOrdering::Less == QVariant::compare(left, right);
}

QString SortedModelVectorView::getSortPropertyNameAsString() const {
    QString name;
    for (const auto &s : sortPropertyName) {
        name += s;
        name += ".";
    }
    if (name.isEmpty()) {
        return name;
    }
    return name.left(name.length() - 1);
}
