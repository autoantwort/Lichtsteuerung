#ifndef MODULEMANAGER_H
#define MODULEMANAGER_H

#include <vector>
#include <functional>
#include "program.hpp"
#include "filter.hpp"
#include "consumer.hpp"
#include <cstring>
#include "property.hpp"

#include <QString>
#include <QFile>
#include <QDir>
#include <QLibrary>
#include <iostream>
#include <memory>
#include "modelvector.h"

namespace Modules {

namespace detail {
    class PropertyInformation : public QObject{
        Q_OBJECT
        QString name = "No Name";
        QString description;
        typedef Property::Type Type;
        Type type;
    public:
        Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
        Q_PROPERTY(QString description READ getDescription WRITE setDescription NOTIFY descriptionChanged)
        Q_PROPERTY(Property::Type type READ getType WRITE setType NOTIFY typeChanged)
        PropertyInformation(const QJsonObject &o);
        void writeJsonObject(QJsonObject &o)const;
        Q_ENUM(Type)
        void setName( const QString _name){
                if(_name != name){
                        name = _name;
                        emit nameChanged();
                }
        }
        QString getName() const {
                return name;
        }
        void setDescription( const QString _description){
                if(_description != description){
                        description = _description;
                        emit descriptionChanged();
                }
        }
        QString getDescription() const {
                return description;
        }
        void setType( const Property::Type _type){
                if(_type != type){
                        type = _type;
                        emit typeChanged();
                }
        }
        Property::Type getType() const {
                return type;
        }
    signals:
        void nameChanged();
        void descriptionChanged();
        void typeChanged();
    };
    class PropertiesVector : public ModelVector<PropertyInformation*>{
      //  Q_OBJECT
    };
}

class Module : public QObject{

    Q_OBJECT
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString description READ getDescription WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(Type type READ getType WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(ValueType valueType READ getValueType WRITE setValueType NOTIFY valueTypeChanged)
    Q_PROPERTY(QString code READ getCode WRITE setCode NOTIFY codeChanged)
    QString name = "No Name";
    QString description;
    detail::PropertiesVector  properties;
    QString code;
    ValueType valueType;
    Q_ENUM(ValueType)

    //Module(Module&)=delete;
public:
    enum Type{Program, LoopProgram, Filter};
    Q_ENUM(Type)
private:
    Type type;
public:
    Module() = default;
    Module(const QJsonObject &o);
    void writeJsonObject(QJsonObject &o)const;

    //Module() = default;
    const detail::PropertiesVector& getProperties()const{
        return properties;
    }

    void setName( const QString _name){
        if(_name != name){
            name = _name;
            emit nameChanged();
        }
    }
    QString getName() const {
        return name;
    }
    void setDescription( const QString _description){
        if(_description != description){
            description = _description;
            emit descriptionChanged();
        }
    }
    QString getDescription() const {
        return description;
    }
    void setType( const Type _type){
        if(_type != type){
            type = _type;
            emit typeChanged();
        }
    }
    Type getType() const {
        return type;
    }
    void setValueType( const ValueType _valueType){
            if(_valueType != valueType){
                    valueType = _valueType;
                    emit valueTypeChanged();
            }
    }
    ValueType getValueType() const {
            return valueType;
    }
    void setCode( const QString _code){
        if(_code != code){
                code = _code;
                emit codeChanged();
        }
    }
    QString getCode() const {
            return code;
    }

signals:
    void nameChanged();
    void descriptionChanged();
    void typeChanged();
    void valueTypeChanged();
    void codeChanged();
};


    namespace detail {
        template<typename T>
        class Entry{
            typedef std::function<T*(void)> CreateFunction;
            CreateFunction createFunc;
        public:
            Entry(const std::string& name,const std::string& description):name(name),description(description){}
            Entry(const std::string& name,const std::string& description,CreateFunction func):createFunc(func),name(name),description(description){}
            const std::string name;
            const std::string description;
            T* create()const{
                if(createFunc)
                    return createFunc();
                return nullptr;
            }
        };
    }

    /**
     * @brief The ModuleManager class load Modules and register them. You con load a Programm/Filter/Consumer by Name from an loaded Module
     */
    class ModuleManager
    {
        typedef std::vector<detail::Entry<Program>> ProgrammModulContainer;
        typedef std::vector<detail::Entry<Filter>> FilterModulContainer;
        typedef std::vector<detail::Entry<Consumer>> ConsumerModulContainer;
        ModelVector<Module*> modules;
        ProgrammModulContainer programms;
        //std::vector<detail::Entry<Programm>> programms;
        FilterModulContainer filter;
        ConsumerModulContainer consumer;
    private:
        template<typename Type, typename String>
        static void loadType(QLibrary & lib, std::vector<detail::Entry<Type>> &c,String name);
    public:
        ModuleManager();
        ~ModuleManager(){for(auto m : modules)delete m;}
        void loadModules(const QJsonObject &o);
        void writeJsonObject(QJsonObject &o);
        static ModuleManager * singletone(){static ModuleManager m;return &m;}
        void loadModule(QString name);
        void loadAllModulesInDir(QDir name);
        ModelVector<Module*>* getModules(){return &modules;}
        const ProgrammModulContainer & getProgrammModules(){return programms;}
        const FilterModulContainer & getFilterModules(){return filter;}
        const ConsumerModulContainer & getConsumerModules(){return consumer;}
        std::shared_ptr<Program> createProgramm(const std::string & name){return createByName(programms,name);}
        std::shared_ptr<Filter>   createFilter  (const std::string & name){return createByName(filter   ,name);}
        std::shared_ptr<Consumer> createConsumer(const std::string & name){return createByName(consumer ,name);}
    protected:
        template<typename T>
        static std::shared_ptr<T> createByName(const std::vector<detail::Entry<T>> & c, const std::string  &name){
            for(auto i = c.cbegin() ; i != c.cend();++i){
                if(i->name == name)
                    return std::shared_ptr<T>(i->create());
            }
            return nullptr;
        }
    };

    typedef unsigned int (*GetNumberOfXXX)();
    typedef char const * (*GetNameOfXXX)(unsigned int index);
    typedef char const * (*GetDescriptionOfXXX)(unsigned int index);


    template<typename Type, typename String >
    void ModuleManager::loadType(QLibrary & lib, std::vector<detail::Entry<Type>> &c , String Typename){
        const auto getNumberOfName = std::string("getNumberOf") + Typename;
        const auto getNameOfName = std::string("getNameOf") + Typename;
        const auto getDescriptionOfName = std::string("getDescriptionOf") + Typename;
        const auto createName = std::string("create") + Typename;

        GetNumberOfXXX getNumberOfProgramms = reinterpret_cast<GetNumberOfXXX>(lib.resolve(getNumberOfName.c_str()));
        GetNameOfXXX getNameOfProgramm  = reinterpret_cast<GetNameOfXXX>(lib.resolve(getNameOfName.c_str()));
        GetDescriptionOfXXX getDescriptionOfProgramm  = reinterpret_cast<GetDescriptionOfXXX>(lib.resolve(getDescriptionOfName.c_str()));
        Type* (*createProgramm)(unsigned int) = reinterpret_cast<Type* (*)(unsigned int)>(lib.resolve(createName.c_str()));
        if(!getNumberOfProgramms || !getNameOfProgramm || !getDescriptionOfProgramm || !createProgramm){
            std::cout << "Functions are missing : " << getNumberOfName <<" : " <<getNumberOfProgramms << " , " <<getNameOfName<<" : " <<getNameOfProgramm<<" , " <<getDescriptionOfName<<" : " <<getDescriptionOfProgramm<<" , " <<createName<<" : " <<createProgramm;
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
