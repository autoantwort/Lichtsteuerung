#ifndef NAMEDOBJECT_H
#define NAMEDOBJECT_H

#include <QJsonObject>
#include <QObject>

namespace DMX {

/**
 * @brief The NamedObject class superclass for Every class that will have a name and a description
 */
class NamedObject : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString description READ getDescription WRITE setDescription NOTIFY descriptionChanged)
private:
    /**
     * @brief name Der Name des Objects
     */
    QString name;
    /**
     * @brief description Eine Beschreibung des Objects, zb um deutlich zu machen, was es macht
     */
    QString description;

public:
    // explicit NamedObject(QObject *parent = 0);
    NamedObject(QString name, QString description = "") : name(name), description(description) {}
    NamedObject(const QJsonObject &o) : name(o["name"].toString()), description(o["description"].toString()) {}

    Q_SLOT void setName(const QString &n) {
        if (n == name) return;
        name = n;
        emit nameChanged(name);
    }
    QString getName() const { return name; }
    Q_SLOT void setDescription(const QString &d) {
        if (d == description) return;
        description = d;
        emit descriptionChanged(description);
    }
    QString getDescription() const { return description; }

    void writeJsonObject(QJsonObject &o) const {
        o.insert("name", name);
        o.insert("description", description);
    }
signals:
    void nameChanged(const QString &);
    void descriptionChanged(const QString &);
};

} // namespace DMX

#endif // NAMEDOBJECT_H
