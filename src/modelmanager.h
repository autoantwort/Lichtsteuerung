#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include "applicationdata.h"
#include "dmx/device.h"
#include "dmx/programm.h"
#include "dmx/programmprototype.h"
#include "modules/modulemanager.h"
#include "settings.h"
#include <QObject>

class ModelManager : public QObject{
    Q_OBJECT
    Settings & settings;
public:
    explicit ModelManager(Settings & settings):settings(settings){}

    Q_INVOKABLE void remove(QObject * item){
        if(item)
            delete item;
    }
    Q_INVOKABLE bool addDevice(int row, int startDMXChannel, QString name, QString desciption="",QPoint position = QPoint(-1,-1)){
        qDebug()<<"addDevice : "<<row<<' ' << startDMXChannel<< ' ' << name << ' '<<desciption<<'\n';
        DMX::DevicePrototype * prototype = IDBaseDataModel<DMX::DevicePrototype>::singletone()->data(row);
        if(prototype){
            new DMX::Device(prototype,startDMXChannel,name,desciption,position);
            return true;
        }
        return false;
    }
    Q_INVOKABLE bool addDevicePrototype(QString name, QString description=""){
        new DMX::DevicePrototype(name,description);
        return true;
    }
    Q_INVOKABLE bool addProgrammPrototype(int row/*DevicePrototype*/,QString name, QString description=""){
        DMX::DevicePrototype * prototype = IDBaseDataModel<DMX::DevicePrototype>::singletone()->data(row);
        if(prototype){
            new DMX::ProgrammPrototype(prototype,name,description);
            return true;
        }
        return false;

    }
    Q_INVOKABLE bool addProgramm(QString name, QString description=""){
        new DMX::Programm(name,description);
        return true;
    }
    Q_INVOKABLE bool addModule(){
        Modules::ModuleManager::singletone()->getModules()->push_back(new Modules::Module());
        qDebug() << "count : "<<Modules::ModuleManager::singletone()->getModules()->size();
        return true;
    }
    Q_INVOKABLE bool duplicateModule(int index){
        const auto vec = Modules::ModuleManager::singletone()->getModules();
        if(index>=0 && index < static_cast<int>(vec->size())){
            QJsonObject o;
            (*vec)[index]->writeJsonObject(o);
            Modules::ModuleManager::singletone()->getModules()->push_back(new Modules::Module(o));
            return true;
        }
        return false;
    }
    Q_INVOKABLE void removeModule(int index){
        const auto vec = Modules::ModuleManager::singletone()->getModules();
        if(index>=0 && index < static_cast<int>(vec->size())){
            delete vec->erase(index);
        }
    }
    Q_INVOKABLE bool addProgramBlock(){
        auto temp = std::make_shared<Modules::ProgramBlock>();
        Modules::ProgramBlockManager::model.push_back(temp);
        return true;
    }
    Q_INVOKABLE void removeProgramBlock(int index){
        auto & vec = Modules::ProgramBlockManager::model;
        if(index>=0 && index < static_cast<int>(vec.size())){
            vec.erase(index);
        }
    }
    /**
      * @brief saves the application data
      */
    Q_INVOKABLE void save(){
        QFile savePath(settings.getJsonSettingsFilePath());
        if(savePath.exists()){
            savePath.copy(savePath.fileName()+"_"+QDateTime::currentDateTime().toString(QStringLiteral("dd.MM.yyyy HH.mm.ss")));
        }
        ApplicationData::saveData(savePath);
    }

};

#endif // MODELMANAGER_H
