#ifndef STORAGE_HPP
#define STORAGE_HPP

namespace Modules {

class SaveObject
{
public:
    virtual void saveInt(const char*name,int) = 0;
    virtual void saveFloat(const char*name,float) = 0;
    virtual void saveDouble(const char*name,double) = 0;
    virtual void saveBool(const char*name,bool) = 0;
    virtual void saveLong(const char*name,long) = 0;
    virtual void saveString(const char*name,const char * c) = 0;
};

class LoadObject
{
public:
    virtual int loadInt(const char*name) const = 0;
    virtual float loadFloat(const char*name) const  = 0;
    virtual double loadDouble(const char*name)  const = 0;
    virtual bool loadBool(const char*name)  const = 0;
    virtual long loadLong(const char*name)  const = 0;
    virtual char* loadStringOwn(const char*name)  const = 0;
};

class Serilerizeable{
public:
    virtual void save(SaveObject &s)const=0;
    virtual void load(const LoadObject &l)=0;
};

}

#endif // STORAGE_HPP
