#include "applicationdata.h"
#include <QJsonDocument>
#include "idbase.h"
#include "channel.h"
#include <QJsonArray>


namespace ApplicationData{

bool saveData(QFile file){
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
    saveIDBaseObjects<Channel>(o,"Channels");
}


}
