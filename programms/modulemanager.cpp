#include "modulemanager.h"
#include <QLibrary>
#include "module.h"
#include <QDebug>
#include "settings.h"

namespace Modules {

typedef  bool (*Have_Func)(MODUL_TYPE t);


typedef unsigned int (*GetNumberOfProgramms)();
typedef std::string (*GetNameOfProgramm)(unsigned int index);
typedef std::string (*GetDescriptionOfProgramm)(unsigned int index);
typedef Modules::Programm* (*CreateProgramm)(unsigned int index);

    ModuleManager::ModuleManager()
    {

    }

    void ModuleManager::loadModule(QString name){
        QLibrary lib(name);
        if(lib.load()){
            Have_Func f = reinterpret_cast<Have_Func>(lib.resolve("have"));
            if(!f){
                qDebug()<<"have funktion is missing";
                return;
            }
            if(f(MODUL_TYPE::Programm)){
                loadType(lib,programms,"Programm");
            }if(f(MODUL_TYPE::LoopProgramm)){
                //loadType(lib,programms,"LoopProgramm");
            }if(f(MODUL_TYPE::Filter)){
                loadType(lib,filter,"Filter");
            }if(f(MODUL_TYPE::Consumer)){
                loadType(lib,consumer,"Consumer");
            }
        }else{
            qDebug()<<"Cant load lib :" << name<< " because : " << lib.errorString();
        }
    }

    void ModuleManager::loadAllModulesInDir(QDir dir){
        for(auto s : dir.entryList(QDir::Files)){
#ifdef Q_OS_WIN
            if(s.suffix() == "dll")
#endif
            loadModule(s.absoluteFilePath());
        }
        for(auto s : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)){
            loadAllModulesInDir(s);
        }
    }

}
