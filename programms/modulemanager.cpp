#include "modulemanager.h"
#include <QLibrary>
#include "module.h"

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
            if(!f)
                return;
            if(f(MODUL_TYPE::Programm)){
                loadType(lib,programms,"Programm");
            }if(f(MODUL_TYPE::LoopProgramm)){
                //loadType(lib,programms,"LoopProgramm");
            }if(f(MODUL_TYPE::Filter)){
                loadType(lib,filter,"Filter");
            }if(f(MODUL_TYPE::Consumer)){
                loadType(lib,consumer,"Consumer");
            }
        }
    }

    void ModuleManager::loadAllModulesInDir(QDir dir){
        for(auto s : dir.entryList(QDir::Files)){
            loadModule(s);
        }
    }

}
