#include "applicationdata.h"
#include <QJsonDocument>
#include "idbase.h"
#include <QJsonArray>

#include "channel.h"
#include "deviceprototype.h"
#include "device.h"
#include "programmprototype.h"
#include "programm.h"

namespace ApplicationData{

bool saveData(QFile &file){
    auto data = saveData();
    if(!file.open(QIODevice::WriteOnly))return false;
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
    QJsonObject o;
    saveIDBaseObjects<DevicePrototype>(o,"DevicePrototypes");
    saveIDBaseObjects<Device>(o,"Devices");
    saveIDBaseObjects<ProgrammPrototype>(o,"ProgrammPrototypes");
    saveIDBaseObjects<Programm>(o,"Programms");
    return QJsonDocument(o).toJson();
}

template<typename Subclass>
void loadIDBaseObjects(const QJsonObject &o,QString entryName ){
    for(const auto e : o[entryName].toArray()){
        new Subclass(e.toObject());
    }
}

bool loadData(QFile &file){
    if(!file.open(QIODevice::ReadOnly)){
        return false;
    }
    loadData(file.readAll());
    file.close();
    return true;
}

void loadData(QByteArray data){
    const auto o = QJsonDocument::fromJson(data).object();
    loadIDBaseObjects<DevicePrototype>(o,"DevicePrototypes");
    loadIDBaseObjects<Device>(o,"Devices");
    loadIDBaseObjects<ProgrammPrototype>(o,"ProgrammPrototypes");
    loadIDBaseObjects<Programm>(o,"Programms");
}


}
