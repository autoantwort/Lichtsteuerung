#include "applicationdata.h"
#include <QJsonDocument>
#include "idbase.h"
#include <QJsonArray>

#include "dmx/channel.h"
#include "dmx/deviceprototype.h"
#include "dmx/device.h"
#include "dmx/programmprototype.h"
#include "dmx/programm.h"
#include "usermanagment.h"
#include "gui/controlpanel.h"
#include "gui/mapview.h"
#include <QCryptographicHash>
#include "programms/modulemanager.h"
#include "programms/programblock.h"
#include "spotify/spotify.h"

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
    saveIDBaseObjects<DMX::DevicePrototype>(o,"DevicePrototypes");
    saveIDBaseObjects<DMX::Device>(o,"Devices");
    saveIDBaseObjects<DMX::ProgrammPrototype>(o,"ProgrammPrototypes");
    saveIDBaseObjects<DMX::Programm>(o,"Programms");
    saveIDBaseObjects<User>(o,"Users");
    o.insert("password",QString::fromLatin1(password.toBase64()));
    {
        QJsonObject u;
        GUI::ControlPanel::getLastCreated()->writeJsonObject(u);
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
    }{
        QJsonObject u;
        Spotify::get().writeToJsonObject(u);
        o.insert("Spotify",u);
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
    loadIDBaseObjects<DMX::DevicePrototype>(o,"DevicePrototypes");
    loadIDBaseObjects<DMX::Device>(o,"Devices");
    loadIDBaseObjects<DMX::ProgrammPrototype>(o,"ProgrammPrototypes");
    loadIDBaseObjects<DMX::Programm>(o,"Programms");
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
    Spotify::get().loadFromJsonObject(o["Spotify"].toObject());
    password=QCryptographicHash::hash(QString("admin").toLatin1(),QCryptographicHash::Sha3_256);
    return [=](){
        GUI::MapView::getLastCreated()->loadFromJsonObject(o["MapView"].toObject());
        Modules::ProgramBlockManager::readFromJsonObject(o["ProgramBlockManager"].toObject());        
        GUI::ControlPanel::getLastCreated()->loadFromJsonObject(o["ControlPanel"].toObject());
    };

}


}
