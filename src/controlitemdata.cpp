#include "controlitemdata.h"
#include <QJsonArray>

ControlItemData::ControlItemData(Type t,QObject *parent) : QObject(parent),type(t)
{

}

ControlItemData::ControlItemData(const QJsonObject &o):
    startXBlock(o["startXBlock"].toInt()),
    startYBlock(o["startYBlock"].toInt()),
    type(static_cast<Type>(o["type"].toInt())){

}

void ControlItemData::writeJsonObject(QJsonObject &o){
    o.insert("type",type);
    o.insert("startXBlock",startXBlock);
    o.insert("startYBlock",startYBlock);
}

void ControlItemData::setStartXBlock(int i){
    if(i!=startXBlock){
        startXBlock=i;
        emit startXBlockChanged();
    }
}
void ControlItemData::setStartYBlock(int i){
    if(i!=startYBlock){
        startYBlock=i;
        emit startYBlockChanged();
    }
}

// start ProgrammControlItemData

ProgrammControlItemData::ProgrammControlItemData(Programm * p,QObject *parent):ControlItemData(ControlItemData::PROGRAMM,parent),programm(p){

}

ProgrammControlItemData::ProgrammControlItemData(const QJsonObject &o):
    ControlItemData(o),
    programm(IDBase<Programm>::getIDBaseObjectByID(o["programm"])){

}

void ProgrammControlItemData::writeJsonObject(QJsonObject &o){
    ControlItemData::writeJsonObject(o);
    o.insert("programm",QString::number(programm->getID().value()));
}
void ProgrammControlItemData::setProgramm(Programm *p){
    if(p!=programm){
        programm = p;
        emit programmChanged();
    }
}

// Group Model

GroupModel::GroupModel(DataModel *deviceModel):deviceModel(deviceModel){
    assert(deviceModel!=nullptr);
    QObject::connect(deviceModel,&DataModel::rowsAboutToBeInserted,this,&GroupModel::handleRowsAboutToBeInserted);
    QObject::connect(deviceModel,&DataModel::rowsAboutToBeRemoved,this,&GroupModel::handleRowsAboutToBeRemoved);
    QObject::connect(deviceModel,&DataModel::rowsInserted,this,&GroupModel::handleRowsInserted);
    QObject::connect(deviceModel,&DataModel::rowsRemoved,this,&GroupModel::handleRowsRemoved);
    enabled.resize(deviceModel->rowCount(QModelIndex()));
}

void GroupModel::handleRowsAboutToBeInserted(const QModelIndex &parent, int start, int end){
    beginInsertRows(parent,start,end);
}
void GroupModel::handleRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end){
    beginRemoveRows(parent,start,end);
}
void GroupModel::handleRowsInserted(const QModelIndex &parent, int first, int last){
    Q_UNUSED(parent);
    enabled.insert(enabled.cbegin()+first,first-last,false);
    endInsertRows();
}
void GroupModel::handleRowsRemoved(const QModelIndex &parent, int first, int last){
    Q_UNUSED(parent)
    enabled.erase(enabled.cbegin()+first,enabled.cend()+last);
    endRemoveRows();
}


// GroupControlItemData
GroupControlItemData::GroupControlItemData(const QJsonObject &o):ControlItemData(o),name(o["name"].toString()){
    const auto enabled = o["enabledDevices"].toArray();
    auto iter = IDBase<Device>::getAllIDBases().cbegin();
    int index = 0;
    for(const auto e_ : enabled){
        const auto e = e_.toString().toLongLong();
        const auto start = iter;
        while (iter!=IDBase<Device>::getAllIDBases().cend()) {
            if((**iter).getID()==e){
                groupModel.setEnabled(index,true);
                ++iter;
                ++index;
                continue;
            }
            ++iter;
            ++index;
        }
        iter = IDBase<Device>::getAllIDBases().cbegin();
        index = 0;
        while (iter!=start) {
            if((**iter).getID()==e){
                groupModel.setEnabled(index,true);
                ++iter;
                ++index;
                continue;
            }
            ++iter;
            ++index;
        }
    }
}

void GroupControlItemData::writeJsonObject(QJsonObject &o){
    QJsonArray a;
    auto prog = IDBase<Device>::getAllIDBases().cbegin();
    auto en = groupModel.getEnabledVector().cbegin();
    for(;en!=groupModel.getEnabledVector().cend();++prog,++en){
        if(*en){
            a.append(QString::number((**prog).getID().value()));
        }
    }
    o.insert("enabledDevices",a);
    o.insert("name",name);
    ControlItemData::writeJsonObject(o);
}

void GroupControlItemData::forEach(std::function<void (Device *)> f){
    auto prog = IDBase<Device>::getAllIDBases().cbegin();
    auto en = groupModel.getEnabledVector().cbegin();
    for(;en!=groupModel.getEnabledVector().cend();++prog,++en){
        if(*en){
            f(*prog);
        }
    }
}

void GroupControlItemData::setName(QString n){
    if(name!=n){
        name = n;
        emit nameChanged();
    }
}
// SwitchGroupControlItemData

SwitchGroupControlItemData::SwitchGroupControlItemData(const QJsonObject &o):GroupControlItemData(o),
    activated(o["activated"].toBool()),
    activateCooldown(o["activateCooldown"].toInt()),
    deactivateCooldown(o["deactivateCooldown"].toInt()){

}

void SwitchGroupControlItemData::writeJsonObject(QJsonObject &o){
    GroupControlItemData::writeJsonObject(o);
    o.insert("activated",activated);
    o.insert("activateCooldown",activateCooldown);
    o.insert("deactivateCooldown",deactivateCooldown);
}

void SwitchGroupControlItemData::setActivateCooldown(int c){
    if(c!=activateCooldown){
        activateCooldown = c;
        emit activateCooldownChanged();
    }
}

void SwitchGroupControlItemData::setDeactivateCooldown(int c){
    if(c!=deactivateCooldown){
        deactivateCooldown = c;
        emit deactivateCooldownChanged();
    }
}

void SwitchGroupControlItemData::setActivated(bool a){
    if(a!=activated){
        activated = a;
        forEach([this](Device * d){
            auto filter = d->getFilterForChannelindex(0);
            if(filter)filter->setValue(255*activated);
        });
        emit activatedChanged();
    }
}

// DimmerGroupControlItemData

DimmerGroupControlItemData::DimmerGroupControlItemData(const QJsonObject &o):GroupControlItemData(o),
    maxOperation(static_cast<Operation>(o["maxOperation"].toInt())),
    minOperation(static_cast<Operation>(o["minOperation"].toInt())),
    maxValue(    o["maxValue"].toInt()),
    minValue(    o["minValue"].toInt()),
    value(       o["value"].toInt()),
    shouldOverrideValue_(o["shouldOverride"].toBool())
{

}

void DimmerGroupControlItemData::writeJsonObject(QJsonObject &o){
    GroupControlItemData::writeJsonObject(o);
    o.insert("maxOperation",maxOperation);
    o.insert("minOperation",minOperation);
    o.insert("maxValue",maxValue);
    o.insert("minValue",minValue);
    o.insert("value",value);
    o.insert("shouldOverride",shouldOverrideValue_);
}

void DimmerGroupControlItemData::setMaxOperation(Operation o){
    if(maxOperation!=o){
        maxOperation = o;
        forEach([=](Device* d){
            auto filter = d->getFilterForChannelindex(0);
            if(filter)filter->setMaxOperation(o);
        });
        emit maxOperationChanged();
    }
}
void DimmerGroupControlItemData::setMinOperation(Operation o){
    if(minOperation!=o){
        minOperation = o;
        forEach([=](Device* d){
            auto filter = d->getFilterForChannelindex(0);
            if(filter)filter->setMinOperation(o);
        });
        emit minOperationChanged();
    }
}

void DimmerGroupControlItemData::setMaxValue(unsigned char o){
    if(maxValue!=o){
        maxValue = o;
        forEach([=](Device* d){
            auto filter = d->getFilterForChannelindex(0);
            if(filter)filter->setMaxValue(o);
        });
        emit maxValueChanged();
    }
}
void DimmerGroupControlItemData::setMinValue(unsigned char o){
    if(minValue!=o){
        minValue = o;
        forEach([=](Device* d){
            auto filter = d->getFilterForChannelindex(0);
            if(filter)filter->setMinValue(o);
        });
        emit minValueChanged();
    }
}

void DimmerGroupControlItemData::setValue(unsigned char o){
    if(value!=o){
        value = o;
        forEach([=](Device* d){
            auto filter = d->getFilterForChannelindex(0);
            if(filter)filter->setValue(o);
        });
        emit valueChanged();
    }
}
void DimmerGroupControlItemData::shouldOverrideValue(bool o){
    if(shouldOverrideValue_!=o){
        shouldOverrideValue_=o;
        forEach([=](Device* d){
            auto filter = d->getFilterForChannelindex(0);
            if(filter)filter->shouldOverrideValue(o);
        });
        emit shouldOverrideValueChanged();
    }
}

