#ifndef JSON_STORAGE_H
#define JSON_STORAGE_H

#include "storage.hpp"
#include <QJsonObject>
#include <functional>
#include <cstring>
namespace Modules {


    /**
     * @brief The JsonLoadObject class is a wrapper around a QJsonObject that implements the LoadObject interface
     */
    class JsonLoadObject : public LoadObject
    {
        const QJsonObject &o;
    public:
        JsonLoadObject(const QJsonObject &o):o(o){}
        virtual int loadInt(const char*name, int defaultValue) const override{return o[name].toInt(defaultValue);}
        virtual float loadFloat(const char*name, float defaultValue) const override {return static_cast<float>(o[name].toDouble(defaultValue));}
        virtual double loadDouble(const char*name, double defaultValue)  const override{return o[name].toDouble(defaultValue);}
        virtual bool loadBool(const char*name, bool defaultValue)  const override{return o[name].toBool(defaultValue);}
        virtual long loadLong(const char*name, long defaultValue)  const override{return static_cast<long>(o[name].toDouble(defaultValue));}
        virtual char* loadStringOwn(const char*name, char* defaultValue)  const override{
            if(o.contains(name)){
                const auto data = o[name].toString().toLatin1();
                char * d = new char[data.size()+1];
                std::memcpy(d,data.constData(),data.size()+1);
                return d;
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
        JsonSaveObject(QJsonObject &o):o(o){}
        virtual void saveInt(const char*name,int i)  override{
            o[name] = i;
        }
        virtual void saveFloat(const char*name,float f)  override{
            o[name] = static_cast<double>(f);
        }
        virtual void saveDouble(const char*name,double d)  override{
            o[name] = d;
        }
        virtual void saveBool(const char*name,bool b)  override{
            o[name] = b;
        }
        virtual void saveLong(const char*name,long l)  override{
            o[name] = static_cast<double>(l);
        }
        virtual void saveString(const char*name,const char * c)  override{
            o[name] = c;
        }
    };

    /**
     * @brief transferProperties transfers the properties to another Serilerizeable
     * @param from the object from where the properties come from
     * @param to the object where the propertie goes to
     */
    void transferProperties(Serilerizeable * from, Serilerizeable * to);



}

#endif // JSON_STORAGE_H