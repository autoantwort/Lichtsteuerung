#include "applicationdata.h"
#include <QJsonDocument>
#include "idbase.h"
#include <QJsonArray>

#include "channel.h"
#include "deviceprototype.h"
#include "device.h"
#include "programmprototype.h"
#include "programm.h"
#include "usermanagment.h"
#include "controlpanel.h"
#include "mapview.h"
#include <QCryptographicHash>
#include "programms/modulemanager.h"
#include "programms/programblock.h"

namespace ApplicationData{


QByteArray password;

bool saveData(QFile &file){
    if(!file.open(QIODevice::WriteOnly))return false;
    auto data = saveData();
    file.write(data);
    file.close();
    return true;
}

template<typename Subclass>
void saveIDBaseObjects(QJsonObject &o,QString entryName ){
    QJsonArray array;
    for(const auto p : IDBase<Subclass>::getAllIDBases()){
        QJsonObject o;
        p->writeJsonObject(o);
        array.append(o);
    }
    o.insert(entryName,array);
}

QByteArray saveData(){
    qDebug()<<"SAVE DATA";
    QJsonObject o;
    saveIDBaseObjects<DevicePrototype>(o,"DevicePrototypes");
    saveIDBaseObjects<Device>(o,"Devices");
    saveIDBaseObjects<ProgrammPrototype>(o,"ProgrammPrototypes");
    saveIDBaseObjects<Programm>(o,"Programms");
    saveIDBaseObjects<User>(o,"Users");
    o.insert("password",QString::fromLatin1(password.toBase64()));
    {
        QJsonObject u;
        ControlPanel::getLastCreated()->writeJsonObject(u);
        o.insert("ControlPanel",u);
    }{
        QJsonObject u;
        GUI::MapView::getLastCreated()->writeJsonObject(u);
        o.insert("MapView",u);
    }{
        QJsonObject u;
        Modules::ModuleManager::singletone()->writeJsonObject(u);
        o.insert("ModuleManager",u);
    }{
        QJsonObject u;
        Modules::ProgramBlockManager::writeToJsonObject(u);
        o.insert("ProgramBlockManager",u);
    }
    return QJsonDocument(o).toJson();
}

template<typename Subclass>
void loadIDBaseObjects(const QJsonObject &o,QString entryName ){
    for(const auto e : o[entryName].toArray()){
        new Subclass(e.toObject());
    }
}

std::function<void()> loadData(QFile &file){
    if(!file.open(QIODevice::ReadOnly)){
        return [](){qDebug()<<"Error: can not open file";};
    }
    auto r = loadData(file.readAll());
    file.close();
    return r;
}


std::function<void()> loadData(QByteArray data){
    const auto o = QJsonDocument::fromJson(data).object();
    loadIDBaseObjects<DevicePrototype>(o,"DevicePrototypes");
    loadIDBaseObjects<Device>(o,"Devices");
    loadIDBaseObjects<ProgrammPrototype>(o,"ProgrammPrototypes");
    loadIDBaseObjects<Programm>(o,"Programms");
    password = QByteArray::fromBase64(o["password"].toString().toLatin1());
    {//USERS
        for(const auto e : o["Users"].toArray()){
            User::createUser(e.toObject());
        }
    }
    if(password==QCryptographicHash::hash(QString("admin").toLatin1(),QCryptographicHash::Sha3_256)){
        qDebug()<<"Erfolgreich";
    }else{
        qDebug()<<"password : "<<password;
        qDebug()<<"hash     : "<<QCryptographicHash::hash(QString("admin").toLatin1(),QCryptographicHash::Sha3_256);
    }
    {
        QJsonObject ob = o["ModuleManager"].toObject();
        Modules::ModuleManager::singletone()->loadModules(ob);
    }
    password=QCryptographicHash::hash(QString("admin").toLatin1(),QCryptographicHash::Sha3_256);
    return [=](){
        ControlPanel::getLastCreated()->loadFromJsonObject(o["ControlPanel"].toObject());
        GUI::MapView::getLastCreated()->loadFromJsonObject(o["MapView"].toObject());
        Modules::ProgramBlockManager::readFromJsonObject(o["ProgramBlockManager"].toObject());
    };

}


}
