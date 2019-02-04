#include "modulemanager.h"
#include <QLibrary>
#include "module.h"
#include <QDebug>
#include "settings.h"
#include <QJsonObject>
#include <QJsonArray>
#include "audio/audiocapturemanager.h"

namespace Modules {

namespace detail {

    PropertyInformation::PropertyInformation(const QJsonObject &o):name(o["name"].toString("no name")),
        description(o["description"].toString()),type(static_cast<Type>(o["type"].toInt())),max(o["maxValue"].toInt()),
        min(o["minValue"].toInt()),defaultValue(o["defaultValue"].toInt()){

    }

    void PropertyInformation::writeJsonObject(QJsonObject &o)const{
            o["type"] = type;
            o["name"] = name;
            o["description"] = description;
            o["minValue"] = min;
            o["maxValue"] = max;
            o["defaultValue"] = defaultValue;
    }

}

Module::Module(const QJsonObject &o):name(o["name"].toString("no name")),
    description(o["description"].toString()),code(o["code"].toString()),
    inputType(static_cast<ValueType>(o["inputType"].toInt())),
    outputType(static_cast<ValueType>(o["outputType"].toInt())),
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
    o["inputType"] = inputType;
    o["outputType"] = outputType;
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
        fftOutputView = Audio::AudioCaptureManager::get().getFFTOutput();
        QObject::connect(&Audio::AudioCaptureManager::get(),&Audio::AudioCaptureManager::capturingStatusChanged,[this](){
            for(const auto & info : loadedLibraryMap){
                if(info.second.supportAudioFunc){
                    info.second.supportAudioFunc(Audio::AudioCaptureManager::get().isCapturing());
                }
            }
        });
    }

    void ModuleManager::loadModules(const QJsonObject & o){
        QJsonArray a = o["modules"].toArray();
        for(const auto _o : a){
            QJsonObject ob = _o.toObject();
            modules.push_back(new Module(ob));
        }
    }

    bool ModuleManager::unloadLibrary(QString filePath){
        for(auto e = loadedLibraryMap.begin();e!=loadedLibraryMap.end();++e){
            qDebug()<<e->first;
            qDebug()<<filePath+"____";
            // add "____" to avoid subname conflicts. eg we unload print ans e have name print_super
            if(e->first.startsWith(filePath+"____",Qt::CaseInsensitive)){
                for(auto it = filter.cbegin();it != filter.cend();){
                    if(it->libraryIdentifier == e->second.libraryIdentifier){
                        it = filter.erase(it);
                    }else{
                        ++it;
                    }
                }
                for(auto it = programms.cbegin();it != programms.cend();){
                    if(it->libraryIdentifier == e->second.libraryIdentifier){
                        it = programms.erase(it);
                    }else{
                        ++it;
                    }
                }
                for(auto it = consumer.cbegin();it != consumer.cend();){
                    if(it->libraryIdentifier == e->second.libraryIdentifier){
                        it = consumer.erase(it);
                    }else{
                        ++it;
                    }
                }
                QLibrary lib(e->first);
                bool suc = lib.unload();
                if(suc){
                   loadedLibraryMap.erase(e);
                   QFile f(filePath);
                   if(!f.remove()){
                       return f.rename(f.fileName()+".old");
                   }
                   return true;
                }
                suc = QFile::rename(e->first,e->first+ ".old");
                if(suc){
                    loadedLibraryMap.erase(e);
                }
                return suc;
            }
        }
        return true;
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

    QString ModuleManager::getFreeAbsoluteFilePathForModule(const QString & name)const{
#ifdef Q_OS_WIN
#define SHARED_LIB_FILE_SUFFIX ".dll"
#else
#define SHARED_LIB_FILE_SUFFIX ".so"
#endif
        for(int counter = 0; ;++counter){
            auto fileName = name + "____" + QString::number(counter)+ SHARED_LIB_FILE_SUFFIX;
            if(!QFileInfo::exists(fileName)&&!QFileInfo::exists(fileName+".old")){
                if(!QFile::rename(name,fileName)){
                    auto msg = ("Renaming from " + name + " to " + fileName + " does not work").toLatin1();
                    qCritical(msg.data());
                }
                return fileName;
            }
        }
    }

    void ModuleManager::loadModule(QString name, bool replaceNewInPBs){

        if(!QLibrary::isLibrary(name))
            return;

        qDebug()<<"load lib  : " << name;
        QLibrary lib(name);
        if(lib.load()){
            Have_Func f = reinterpret_cast<Have_Func>(lib.resolve("have"));
            if(!f){
                qDebug()<<"have funktion is missing";
                return;
            }
            if(f(MODUL_TYPE::Program)){
                loadType(lib,programms,"Program",lastLibraryIdentifier,[&](const auto p){
                    if(replaceNewInPBs){
                        for(const auto & pb : ProgramBlockManager::model){
                            for(const auto & v : pb->getUsedProgramsByName(p->name())){
                                pb->replaceProgram(v,std::shared_ptr<Program>(p->create()));
                            }
                        }
                    }
                });
            }if(f(MODUL_TYPE::LoopProgram)){
                loadType(lib,programms,"LoopProgram",lastLibraryIdentifier,[&](const auto p){
                    if(replaceNewInPBs){
                        for(const auto & pb : ProgramBlockManager::model){
                            for(const auto & v : pb->getUsedProgramsByName(p->name())){
                                pb->replaceProgram(v,std::shared_ptr<Program>(p->create()));
                            }
                        }
                    }
                });
            }if(f(MODUL_TYPE::Filter)){
                loadType(lib,filter,"Filter",lastLibraryIdentifier,[&](const auto p){
                    if(replaceNewInPBs){
                        for(const auto & pb : ProgramBlockManager::model){
                            for(const auto & v : pb->getUsedFiltersByName(p->name())){
                                pb->replaceFilter(v,std::shared_ptr<Filter>(p->create()));
                            }
                        }
                    }
                });
            }if(f(MODUL_TYPE::Consumer)){
                qDebug()<< "Loading Consumer";
                loadType(lib,consumer,"Consumer",lastLibraryIdentifier,[&](const auto p){
                    if(replaceNewInPBs){
                        for(const auto & pb : ProgramBlockManager::model){
                            for(const auto & v : pb->getUsedConsumersByName(p->name())){
                                pb->replaceConsumer(v,std::shared_ptr<Consumer>(p->create()));
                            }
                        }
                    }
                });
            }
            SupportAudioFunc supportAudioFunc = nullptr;
            if(f(MODUL_TYPE::Audio)){
                //qDebug()<< "Loading Audio";
                supportAudioFunc = loadAudio(lib,&fftOutputView);
            }
            lastLibraryIdentifier++;
            loadedLibraryMap.emplace_back(lib.fileName(),LibInfo{lastLibraryIdentifier,supportAudioFunc});
            qDebug() << lib.errorString();
        }else{
            qDebug()<<"Cant load lib :" << name<< " because : " << lib.errorString() ;
        }
    }

    void ModuleManager::loadAllModulesInDir(QDir dir){
         qDebug() << dir;
        for(auto s : dir.entryInfoList(QDir::Files)){
            if(s.suffix() == "old"){
                QFile::remove(s.filePath());
            }
            if(QLibrary::isLibrary(s.fileName())){
                loadModule(s.absoluteFilePath());
            }
            //qDebug()<<"found : " << s;
        }
        for(auto s : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)){
            loadAllModulesInDir(s);
        }
        for(const auto & m : getProgrammModules())
            qDebug() << m.name().c_str();
        for(const auto & m : getFilterModules())
            qDebug() << m.name().c_str();
        for(const auto & m : getConsumerModules())
            qDebug() << m.name().c_str();

    }



    ModuleManager::SupportAudioFunc ModuleManager::loadAudio(QLibrary & lib, Modules::FFTOutputView<float> * fftOutputView){
        //typedef void (*SupportAudioFunc )(bool);
        typedef void (*SetFFTOutputViewFunc )(Modules::FFTOutputView<float>*);
        //using SupportAudioFunc = unsigned int (bool);
        //using SetFFTOutputViewFunc = char const * (Modules::FFTOutputView<float>*);
        auto supportAudioFunc = reinterpret_cast<SupportAudioFunc>(lib.resolve("_supportAudio"));
        auto setFFTOutputViewFunc = reinterpret_cast<SetFFTOutputViewFunc>(lib.resolve("_setFFTOutputView"));
        if (!supportAudioFunc || !setFFTOutputViewFunc) {
            qDebug() << "Error loading audio functions : "<< supportAudioFunc << setFFTOutputViewFunc;
            return nullptr;
        }
        setFFTOutputViewFunc(fftOutputView);
        supportAudioFunc(Audio::AudioCaptureManager::get().isCapturing());
        return  supportAudioFunc;
    }

}

