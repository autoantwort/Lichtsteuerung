#ifndef MODULEMANAGER_H
#define MODULEMANAGER_H

#include <vector>
#include <functional>
#include "program.hpp"
#include "filter.hpp"
#include "consumer.hpp"
#include <cstring>
#include "property.hpp"
#include "modules/loopprogram.hpp"
#include "modules/boostloopprogramcontextswitcher.h"

#include <QString>
#include <QFile>
#include <QDir>
#include <QLibrary>
#include <iostream>
#include <memory>
#include "modelvector.h"
#include "controller.h"
#include "fftoutput.hpp"
#include "spotify/spotify.h"

namespace Modules {

namespace detail {
    class PropertyInformation : public QObject{
        Q_OBJECT
        QString name = "no_name";
        QString description;
    public:
        typedef Property::Type Type;
    private:
        Type type;
        Q_ENUM(Type)
        int max=1000,min=0,defaultValue=1;
    public:
        Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
        Q_PROPERTY(QString description READ getDescription WRITE setDescription NOTIFY descriptionChanged)
        Q_PROPERTY(Type type READ getType WRITE setType NOTIFY typeChanged)
        Q_PROPERTY(int minValue MEMBER min NOTIFY minChanged)
        Q_PROPERTY(int maxValue MEMBER max NOTIFY maxChanged)
        Q_PROPERTY(int defaultValue MEMBER defaultValue NOTIFY defaultChanged)
        PropertyInformation() = default;
        PropertyInformation(const QJsonObject &o);
        void writeJsonObject(QJsonObject &o)const;
        void setName( const QString _name){
                if(_name != name){
                        name = _name;
                        emit nameChanged();
                }
        }
        int getMinValue()const{return min;}
        int getMaxValue()const{return max;}
        int getDefaultValue()const{return defaultValue;}
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
        void minChanged();
        void maxChanged();
        void defaultChanged();
    };
}

class PropertiesVector : public ModelVector<detail::PropertyInformation*>{
    Q_OBJECT
};

class Module : public QObject{

    Q_OBJECT
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString compiledName READ getCompiledName WRITE setCompiledName NOTIFY compiledNameChanged)
    Q_PROPERTY(QString description READ getDescription WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(Type type READ getType WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(ValueType inputType READ getInputType WRITE setInputType NOTIFY inputTypeChanged)
    Q_PROPERTY(ValueType outputType READ getOutputType WRITE setOutputType NOTIFY outputTypeChanged)
    Q_PROPERTY(QString code READ getCode WRITE setCode NOTIFY codeChanged)
    Q_PROPERTY(bool spotifyResponder MEMBER spotifyResponder NOTIFY spotifyResponderValueChanged)
    Q_PROPERTY(PropertiesVector * properties READ getPropertiesP CONSTANT)
    QString name = "No_Name";
    /**
     * @brief compiledName Der Name, der beim letzten kompilieren benutzt wurde und wie das Module jetzt in der dll heißt. Kann ein anderer sein als in <name>
     */
    QString compiledName = "";
    QString description;
    PropertiesVector  properties;
    QString code;
    ValueType inputType;
    ValueType outputType;
    /**
     * @brief spotifyResponder indicates if the module have functions like onBeat onTatum ...
     */
    bool spotifyResponder = false;
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

    Q_INVOKABLE QVariant createNewProperty(){
        auto p = new detail::PropertyInformation();
        properties.push_back(p);
        qDebug() <<"num elements : " << properties.size();
        return QVariant::fromValue(p);
    }

    Q_INVOKABLE void removeProperty(QVariant v){
        assert(v.canConvert(qMetaTypeId<detail::PropertyInformation*>()));
        for(auto i = properties.cbegin();i!=properties.cend();++i){
            if(*i == v.value<detail::PropertyInformation*>()){
                properties.erase(i);
                delete v.value<detail::PropertyInformation*>();
                return;
            }
        }
    }

    //Module() = default;
    const PropertiesVector& getProperties()const{
        return properties;
    }

    bool isSpotifyResponder()const{return spotifyResponder;}

    PropertiesVector * getPropertiesP(){
        return &properties;
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

    void setCompiledName( const QString &_compiledName){
        if(_compiledName != compiledName){
            compiledName = _compiledName;
            emit compiledNameChanged();
        }
    }
    QString getCompiledName() const {
        return compiledName;
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
    void setInputType( const ValueType _inputType){
            if(_inputType != inputType){
                    inputType = _inputType;
                    emit inputTypeChanged();
            }
    }
    ValueType getInputType() const {
            return inputType;
    }
    void setOutputType( const ValueType _outputType){
            if(_outputType != outputType){
                    outputType = _outputType;
                    emit outputTypeChanged();
            }
    }
    ValueType getOutputType() const {
            return outputType;
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
    void compiledNameChanged();
    void descriptionChanged();
    void typeChanged();
    void inputTypeChanged();
    void outputTypeChanged();
    void codeChanged();
    void spotifyResponderValueChanged();
};


    namespace detail {
        template<typename T>
        class Entry{
            typedef std::function<T*(void)> CreateFunction;
            CreateFunction createFunc;
            std::string name_;
            std::string description_;
        public:
            int libraryIdentifier = -1;
            Entry(const std::string& name,const std::string& description,int libraryIdentifier = -1, CreateFunction func = nullptr):createFunc(func),name_(name),description_(description),libraryIdentifier(libraryIdentifier){}
            const std::string& name()const{return name_;}
            const std::string& description()const{return description_;}
            T* create()const{
                if(createFunc)
                    return createFunc();
                return nullptr;
            }
            bool operator<(const Entry<T> &other)const{
                return this->name_ < other.name_;
            }
        };
    }

    /**
     * @brief The ModuleManager class load Modules and register them. You con load a Programm/Filter/Consumer by Name from an loaded Module
     */
    class ModuleManager
    {
        template<typename T>
        using ModuleContainer = std::set<T>;
        typedef ModuleContainer<detail::Entry<Program>> ProgrammModuleContainer;
        typedef ModuleContainer<detail::Entry<Filter>> FilterModuleContainer;
        typedef ModuleContainer<detail::Entry<Consumer>> ConsumerModuleContainer;
        using SupportAudioFunc = void (*)(bool);
        using SetFFTOutputViewFunc = void (*)(Modules::FFTOutputView<float> *);
        struct LibInfo{
            int libraryIdentifier = -1;
            SupportAudioFunc supportAudioFunc = nullptr;
            SetFFTOutputViewFunc setFFTOutputViewFunc = nullptr;
        };
        std::vector<std::pair<QString,LibInfo>> loadedLibraryMap;
        int lastLibraryIdentifier = 0;
        ModelVector<Module*> modules;
        ProgrammModuleContainer programms;
        //std::vector<detail::Entry<Programm>> programms;
        FilterModuleContainer filter;
        ConsumerModuleContainer consumer;
        Controller controller_;
        FFTOutputView<float> fftOutputView;
        QMetaObject::Connection captureStatusChangedConnection;
    private:
        template<typename Type, typename String, typename Callback>
        static void loadType(QLibrary & lib, ModuleContainer<detail::Entry<Type>> &c,String name, int libraryIdentifier, Callback = nullptr);

        static std::tuple<SupportAudioFunc, SetFFTOutputViewFunc> loadAudio(QLibrary &lib, Modules::FFTOutputView<float> *fftOutputView);

    public:
        ModuleManager();
        ~ModuleManager(){for(auto m : modules)delete m;QObject::disconnect(captureStatusChangedConnection);}
        void setSpotify(Spotify::Spotify * s);
        /**
         * @brief getFreeAbsoluteFilePathForModule if you have a new compiled module that is not loaded, you can get here a new free name.
         * @note Important! If you use an old file name, maybe the old lib gets loaded, even when you delete the file
         * @param moduleFilePath
         * @return
         */
        QString getFreeAbsoluteFilePathForModule(const QString &moduleFilePath)const;
        void loadModules(const QJsonObject &o);
        bool unloadLibrary(QString filePath);
        void writeJsonObject(QJsonObject &o);
        Controller & controller(){return controller_;}
        static ModuleManager * singletone(){static ModuleManager m;return &m;}
        /**
         * @brief loadModule loads the shared library(module) given by the filePath, if fileNamePath is no shared lib, nothing will happen
         * @param fileNamePath the filePath of the shared library that should be loaded
         * @param replaceOldModulesInProgramBlocks if new programs/filters/consumers are loaded old programs/filters/consumers can be replaced, but they can now have another name, so you have to provide a function that maps new names to the old names to replace them. If no function is provided, no modules gets replaced
         */
        void loadModule(QString fileNamePath, std::function<std::string(const std::string&)> replaceOldModulesInProgramBlocks = std::function<std::string(const std::string&)>());
        void loadAllModulesInDir(const QDir &name);
        ModelVector<Module*>* getModules(){return &modules;}
        const ProgrammModuleContainer & getProgrammModules(){return programms;}
        const FilterModuleContainer & getFilterModules(){return filter;}
        const ConsumerModuleContainer & getConsumerModules(){return consumer;}
        std::shared_ptr<Program> createProgramm(const std::string & name){return createByName(programms,name);}
        std::shared_ptr<Filter>   createFilter  (const std::string & name){return createByName(filter   ,name);}
        std::shared_ptr<Consumer> createConsumer(const std::string & name){return createByName(consumer ,name);}
    protected:
        template<typename T>
        static std::shared_ptr<T> createByName(const std::set<detail::Entry<T>> & c, const std::string  &name){
            for(auto i = c.cbegin() ; i != c.cend();++i){
                if(i->name() == name)
                    return std::shared_ptr<T>(i->create());
            }
            return nullptr;
        }
    };

    typedef unsigned int (*GetNumberOfXXX)();
    typedef char const * (*GetNameOfXXX)(unsigned int index);
    typedef char const * (*GetDescriptionOfXXX)(unsigned int index);


    template<typename Type, typename String,typename Callback >
    void ModuleManager::loadType(QLibrary & lib, ModuleContainer<detail::Entry<Type>> &c , String Typename, int libraryIdentifier, Callback callback){
        const auto getNumberOfName = std::string("getNumberOf") + Typename + "s";
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
            std::function<Type*(void)> func = [](){return nullptr;};
            if("LoopProgram"s == Typename){
                func = [=]() -> Type*{
                    LoopProgram * lp = static_cast<LoopProgram*>(static_cast<void*>(createProgramm(i)));
                    lp->setContextSwitcher(std::make_unique<BoostLoopProgramContextSwitcher>());
                    return static_cast<Type*>(static_cast<void*>(lp));
                };
            }else{
                func = [=](){return createProgramm(i);};
            }
            auto iter = c.find({name,desc});
            const detail::Entry<Type> * p;
            if(iter!=c.cend()){ // remove old entry if exists
                iter = c.erase(iter);
                auto tempIter = c.emplace(name,desc,libraryIdentifier,func).first;
                Q_ASSERT(tempIter != c.cend());
                p = &*tempIter;
            }else{
                p = &*c.emplace(name,desc,libraryIdentifier,func).first;
            }
            callback(p);
        }
    }

}
Q_DECLARE_METATYPE(Modules::detail::PropertyInformation::Type)
Q_DECLARE_METATYPE(Modules::PropertiesVector*)
#endif // MODULEMANAGER_H
