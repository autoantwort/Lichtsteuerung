#include "modulemanager.h"
#include <QLibrary>
#include "module.h"
#include <QDebug>
#include "settings.h"
#include <QJsonObject>
#include <QJsonArray>

namespace Modules {

namespace detail {

    PropertyInformation::PropertyInformation(const QJsonObject &o):name(o["name"].toString("no name")),
        description(o["description"].toString()),type(static_cast<Type>(o["type"].toInt())){

    }

    void PropertyInformation::writeJsonObject(QJsonObject &o)const{
            o["type"] = type;
            o["name"] = name;
            o["description"] = description;
    }

}

Module::Module(const QJsonObject &o):name(o["name"].toString("no name")),
    description(o["description"].toString()),code(o["code"].toString()),
    valueType(static_cast<ValueType>(o["valueType"].toInt())),
    type(static_cast<Type>(o["type"].toInt())){
    QJsonArray a = o["properties"].toArray();
    for(const auto i : a){
        QJsonObject ob = i.toObject();
        properties.push_back(new detail::PropertyInformation(ob));
    }
}

void Module::writeJsonObject(QJsonObject &o)const{
    o["name"] = name;
    o["description"] = description;
    o["code"] = code;
    o["valueType"] = valueType;
    o["type"] = type;
    QJsonArray a;
    for(const auto & p : properties){
        QJsonObject prop;
        p->writeJsonObject(prop);
        a.append(prop);
    }
    o["properties"] = a;
}

typedef  bool (*Have_Func)(MODUL_TYPE t);


typedef unsigned int (*GetNumberOfProgramms)();
typedef std::string (*GetNameOfProgramm)(unsigned int index);
typedef std::string (*GetDescriptionOfProgramm)(unsigned int index);
typedef Modules::Program* (*CreateProgramm)(unsigned int index);

    ModuleManager::ModuleManager()
    {

    }

    void ModuleManager::loadModules(const QJsonObject & o){
        QJsonArray a = o["modules"].toArray();
        for(const auto _o : a){
            QJsonObject ob = _o.toObject();
            modules.push_back(new Module(ob));
        }
    }

    void ModuleManager::writeJsonObject(QJsonObject &o){
        QJsonArray a ;
        for(const auto &m : modules){
            QJsonObject ob;
            m->writeJsonObject(ob);
            a.append(ob);
        }
        o["modules"] = a;
    }

    void ModuleManager::loadModule(QString name){
        qDebug()<<"load lib  : " << name;
        QLibrary lib(name);
        if(lib.load()){
            Have_Func f = reinterpret_cast<Have_Func>(lib.resolve("have"));
            if(!f){
                qDebug()<<"have funktion is missing";
                return;
            }
            if(f(MODUL_TYPE::Program)){
                loadType(lib,programms,"Programm");
            }if(f(MODUL_TYPE::LoopProgram)){
                //loadType(lib,programms,"LoopProgramm");
            }if(f(MODUL_TYPE::Filter)){
                loadType(lib,filter,"Filter");
            }if(f(MODUL_TYPE::Consumer)){
                qDebug()<< "Loading Consumer";
                loadType(lib,consumer,"Consumer");
            }
        }else{
            qDebug()<<"Cant load lib :" << name<< " because : " << lib.errorString();
        }
    }

    void ModuleManager::loadAllModulesInDir(QDir dir){
         qDebug() << dir;
        for(auto s : dir.entryInfoList(QDir::Files)){
#ifdef Q_OS_WIN
            if(s.suffix() == "dll")
#endif
            loadModule(s.absoluteFilePath());
            qDebug()<<"found : " << s;
        }
        for(auto s : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)){
            loadAllModulesInDir(s);
        }
        for(const auto & m : getProgrammModules())
            qDebug() << m.name.c_str();
        for(const auto & m : getFilterModules())
            qDebug() << m.name.c_str();
        for(const auto & m : getConsumerModules())
            qDebug() << m.name.c_str();
    }

}
