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
#include "modules/modulemanager.h"
#include "modules/programblock.h"
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

template<typename Vector>
void saveIDBaseObjects(QJsonObject &o,const Vector & vector, QString entryName ){
    QJsonArray array;
    for(const auto & p : vector){
        QJsonObject o;
        p->writeJsonObject(o);
        array.append(o);
    }
    o.insert(entryName,array);
}

QByteArray saveData(){
    qDebug()<<"SAVE DATA";
    QJsonObject o;
    saveIDBaseObjects(o,ModelManager::get().getDevicePrototypes(),"DevicePrototypes");
    saveIDBaseObjects(o,ModelManager::get().getDevices(),"Devices");
    saveIDBaseObjects(o,ModelManager::get().getProgramPrototypes(),"ProgrammPrototypes");
    saveIDBaseObjects(o,ModelManager::get().getPrograms(),"Programms");
    saveIDBaseObjects(o,UserManagment::get()->getUsers(),"Users");
    o.insert("password",QString::fromLatin1(password.toBase64()));
    if (GUI::ControlPanel::getLastCreated()) {
        QJsonObject u;
        GUI::ControlPanel::getLastCreated()->writeJsonObject(u);
        o.insert("ControlPanel",u);
    }
    if (GUI::MapView::getLastCreated()) {
        QJsonObject u;
        GUI::MapView::getLastCreated()->writeJsonObject(u);
        o.insert("MapView",u);
    }
    {
        QJsonObject u;
        Modules::ModuleManager::singletone()->writeJsonObject(u);
        o.insert("ModuleManager",u);
    }
    {
        QJsonObject u;
        Modules::ProgramBlockManager::writeToJsonObject(u);
        o.insert("ProgramBlockManager",u);
    }
    {
        QJsonObject u;
        Spotify::get().writeToJsonObject(u);
        o.insert("Spotify",u);
    }
    return QJsonDocument(o).toJson();
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
    for(const auto e : o["DevicePrototypes"].toArray()){
        ModelManager::get().addNewDevicePrototype(e.toObject());
    }
    for(const auto e : o["Devices"].toArray()){
        ModelManager::get().addNewDevice(e.toObject());
    }
    for(const auto e : o["ProgrammPrototypes"].toArray()){
        ModelManager::get().addNewProgramPrototype(e.toObject());
    }
    for(const auto e : o["Programms"].toArray()){
        ModelManager::get().addNewProgram(e.toObject());
    }
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
    return [=]() {
        if (GUI::MapView::getLastCreated()) GUI::MapView::getLastCreated()->loadFromJsonObject(o["MapView"].toObject());
        Modules::ProgramBlockManager::readFromJsonObject(o["ProgramBlockManager"].toObject());
        if (GUI::ControlPanel::getLastCreated()) GUI::ControlPanel::getLastCreated()->loadFromJsonObject(o["ControlPanel"].toObject());
    };
}
}
