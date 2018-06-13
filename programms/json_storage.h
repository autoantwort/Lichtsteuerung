#ifndef JSON_STORAGE_H
#define JSON_STORAGE_H

#include "storage.hpp"
#include <QJsonObject>
#include <functional>
namespace Modules {


    /**
     * @brief The JsonLoadObject class is a wrapper around a QJsonObject that implements the LoadObject interface
     */
    class JsonLoadObject : public LoadObject
    {
        const QJsonObject &o;
    public:
        JsonLoadObject(const QJsonObject &o):o(o){}
        virtual int loadInt(const char*name) const override{return o[name].toInt();}
        virtual float loadFloat(const char*name) const override {return static_cast<float>(o[name].toDouble());}
        virtual double loadDouble(const char*name)  const override{return o[name].toDouble();}
        virtual bool loadBool(const char*name)  const override{return o[name].toBool();}
        virtual long loadLong(const char*name)  const override{return static_cast<long>(o[name].toDouble());}
        virtual char* loadStringOwn(const char*name)  const override{
            const auto data = o[name].toString().toLatin1();
            char * d = new char[data.size()+1];
            std::memcpy(d,data.constData(),data.size()+1);
            return d;
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
}

#endif // JSON_STORAGE_H
