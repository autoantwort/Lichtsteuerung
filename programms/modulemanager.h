#ifndef MODULEMANAGER_H
#define MODULEMANAGER_H

#include <vector>
#include <functional>
#include "programm.h"
#include "filter.h"
#include "consumer.h"
#include <cstring>

#include <QString>
#include <QFile>
#include <QDir>
#include <QLibrary>

namespace Modules {

    namespace detail {
        template<typename T>
        class Entry{
            typedef std::function<T*(void)> CreateFunction;
            CreateFunction createFunc;
        public:
            Entry(const std::string& name,const std::string& description):name(name),description(description){}
            Entry(const std::string& name,const std::string& description,CreateFunction func):name(name),description(description),createFunc(func){}
            const std::string name;
            const std::string description;
            T* create()const{
                if(createFunc)
                    return createFunc();
                return nullptr;
            }
        };
    }

    class ModuleManager
    {
        typedef std::vector<detail::Entry<Programm>> ProgrammModulContainer;
        typedef std::vector<detail::Entry<Filter>> FilterModulContainer;
        typedef std::vector<detail::Entry<Consumer>> ConsumerModulContainer;
        ProgrammModulContainer programms;
        //std::vector<detail::Entry<Programm>> programms;
        FilterModulContainer filter;
        ConsumerModulContainer consumer;
    private:
        template<typename Type, typename String>
        static void loadType(QLibrary & lib, std::vector<detail::Entry<Type>> &c,String name);
    public:
        ModuleManager();
        void loadModule(QString name);
        void loadAllModulesInDir(QDir name);
        const ProgrammModulContainer & getProgrammModules(){return programms;}
        const FilterModulContainer & getFilterModules(){return filter;}
        const ConsumerModulContainer & getConsumerModules(){return consumer;}
        Programm * createProgramm(const std::string & name){return createByName(programms,name);}
        Filter   * createFilter  (const std::string & name){return createByName(filter   ,name);}
        Consumer * createConsumer(const std::string & name){return createByName(consumer ,name);}
    protected:
        template<typename T>
        static T * createByName(const std::vector<detail::Entry<T>> & c, const std::string  &name){
            for(auto i = c.cbegin() ; i != c.cend();++i){
                if(i->name == name)
                    return i->create();
            }
            return nullptr;
        }
    };

    typedef unsigned int (*GetNumberOfXXX)();
    typedef std::string (*GetNameOfXXX)(unsigned int index);
    typedef std::string (*GetDescriptionOfXXX)(unsigned int index);


    template<typename Type, typename String >
    void ModuleManager::loadType(QLibrary & lib, std::vector<detail::Entry<Type>> &c , String Typename){
        const auto getNumberOfName = std::string("getNumberOf") + Typename;
        const auto getNameOfName = std::string("getNameOf") + Typename;
        const auto getDescriptionOfName = std::string("getDescriptionO") + Typename;
        const auto createName = std::string("create") + Typename;

        GetNumberOfXXX getNumberOfProgramms = reinterpret_cast<GetNumberOfXXX>(lib.resolve(getNumberOfName.c_str()));
        GetNameOfXXX getNameOfProgramm  = reinterpret_cast<GetNameOfXXX>(lib.resolve(getNameOfName.c_str()));
        GetDescriptionOfXXX getDescriptionOfProgramm  = reinterpret_cast<GetDescriptionOfXXX>(lib.resolve(getDescriptionOfName.c_str()));
        Type* (*createProgramm)(unsigned int) = reinterpret_cast<Type* (*)(unsigned int)>(lib.resolve(createName.c_str()));
        if(!getNumberOfProgramms || !getNameOfProgramm || !getDescriptionOfProgramm || !createProgramm){
            return;
        }
        for(unsigned int i = 0 ; i < getNumberOfProgramms();++i){
            const auto name = getNameOfProgramm(i);
            const auto desc = getDescriptionOfProgramm(i);
            const auto func = [=](){return createProgramm(i);};
            c.emplace_back(name,desc,func);
        }
    }

}
#endif // MODULEMANAGER_H
