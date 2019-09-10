#ifndef JSON_STORAGE_H
#define JSON_STORAGE_H

#include "storage.hpp"
#include <QJsonObject>
#include <cstring>
#include <functional>

namespace Modules {


    /**
     * @brief The JsonLoadObject class is a wrapper around a QJsonObject that implements the LoadObject interface
     */
    class JsonLoadObject : public LoadObject
    {
        const QJsonObject &o;
    public:
        explicit JsonLoadObject(const QJsonObject &o) : o(o) {}
        int loadInt(const char *name, int defaultValue) const override { return o[name].toInt(defaultValue); }
        float loadFloat(const char *name, float defaultValue) const override { return static_cast<float>(o[name].toDouble(static_cast<double>(defaultValue))); }
        double loadDouble(const char *name, double defaultValue) const override { return o[name].toDouble(defaultValue); }
        bool loadBool(const char *name, bool defaultValue) const override { return o[name].toBool(defaultValue); }
        int64_t loadLong(const char *name, int64_t defaultValue) const override {
            bool ok;
            auto r = o[name].toString().toLongLong(&ok);
            return ok ? r : defaultValue;
        }
        std::string loadString(const char *name, std::string defaultValue) const override {
            if(o.contains(name)){
                return o[name].toString().toStdString();
            }
            return defaultValue;
        }
    };

    /**
     * @brief The JsonSaveObject class is a wrapper around a QJsonObject that implements the SaveObject interface
     */
    class JsonSaveObject : public SaveObject{
        QJsonObject &o;
    public:
        explicit JsonSaveObject(QJsonObject &o) : o(o) {}
        void saveInt(const char *name, int i) override { o[name] = i; }
        void saveFloat(const char *name, float f) override { o[name] = static_cast<double>(f); }
        void saveDouble(const char *name, double d) override { o[name] = d; }
        void saveBool(const char *name, bool b) override { o[name] = b; }
        void saveLong(const char *name, int64_t l) override { o[name] = QString::number(l); }
        void saveString(const char *name, const char *c) override { o[name] = c; }
    };

    /**
     * @brief transferProperties transfers the properties to another Serilerizeable
     * @param from the object from where the properties come from
     * @param to the object where the propertie goes to
     */
    void transferProperties(Serilerizeable *from, Serilerizeable *to);

    } // namespace Modules

#endif // JSON_STORAGE_H
