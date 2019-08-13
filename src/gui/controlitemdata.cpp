#include "controlitemdata.h"
#include <QJsonArray>
#include <QQmlEngine>

namespace GUI{


///////////////////////////////////////////////////
//////////   UserVisibilityModel   ////////////////
///////////////////////////////////////////////////

UserVisibilityModel::UserVisibilityModel(const QJsonObject &o){
    const auto array = o["excludedUserIDs"].toArray();
    for(const auto o : array){
        const auto obj = o.toObject();
        excudedUsers.emplace(obj);
    }
}

void UserVisibilityModel::writeJsonObject(QJsonObject &o)const{
    QJsonArray a;
    for(const auto & id : excudedUsers){
        QJsonObject o;
        id.writeJsonObject(o);
        a.push_back(o);
    }
    o["excludedUserIDs"] = a;
}

bool UserVisibilityModel::isVisibleForCurrentUser() const{
    if (UserManagment::get()->getCurrentUser()->havePermission(UserManagment::Admin)) {
        return true;
    }
    return excudedUsers.find(UserManagment::get()->getCurrentUser()->getID()) == excudedUsers.end();
}

QVariant UserVisibilityModel::data(const QModelIndex &index, int role) const{
    if(index.row()<0 && index.row()>=rowCount(index)){
        return QVariant("Index Out of Range");
    }
    switch (role) {
    case VisibilityRole:
    case Qt::EditRole:
        return QVariant(excudedUsers.find(UserManagment::get()->getUsers()[index.row()]->getID()) == excudedUsers.end());
    case Qt::DisplayRole:
    case UserNameRole:
        return QVariant(UserManagment::get()->getUsers()[index.row()]->getUsername());
    default:
        return QVariant("Unknown role!");
    }
}

bool UserVisibilityModel::setData(const QModelIndex &index, const QVariant &value, int role){
    if(role==VisibilityRole){
        if(index.row()>=0 && index.row()<rowCount(index)){
            const auto id = UserManagment::get()->getUsers()[index.row()]->getID();
            if (value.toBool()) {
                excudedUsers.erase(excudedUsers.find(id));
            }else{
                const bool isAdmin = UserManagment::get()->getUsers()[index.row()]->havePermission(UserManagment::Admin);
                if(!isAdmin){
                    excudedUsers.insert(id);
                }else{
                    emit dataChanged(this->index(index.row()),this->index(index.row()),{VisibilityRole});
                }
            }
            return true;
        }
    }
    return false;
}

///////////////////////////////////////////////////
////////////   ControlItemData   //////////////////
///////////////////////////////////////////////////

ControlItemData::ControlItemData(Type t,QObject *parent) : QObject(parent),  type(t)
{
    QObject::connect(UserManagment::get(),&UserManagment::currentUserChanged,[this](){
        emit isVisibleForUserChanged();
    });
}

ControlItemData::ControlItemData(const QJsonObject &o, QObject *parent):
    QObject(parent),
    startXBlock(o["startXBlock"].toInt()),
    startYBlock(o["startYBlock"].toInt()),
    userVisibilityModel(o),
    type(static_cast<Type>(o["type"].toInt())){
    QObject::connect(UserManagment::get(),&UserManagment::currentUserChanged,[this](){
        emit isVisibleForUserChanged();
    });
}

void ControlItemData::writeJsonObject(QJsonObject &o){
    o.insert("type",type);
    o.insert("startXBlock",startXBlock);
    o.insert("startYBlock",startYBlock);
    userVisibilityModel.writeJsonObject(o);
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

ProgrammControlItemData::ProgrammControlItemData(DMX::Programm * p,QObject *parent):ControlItemData(ControlItemData::PROGRAMM,parent),programm(p){

}

ProgrammControlItemData::ProgrammControlItemData(const QJsonObject &o, QObject *parent):
    ControlItemData(o,parent),
    programm(ModelManager::get().getProgramById(o["programm"])){

}

void ProgrammControlItemData::writeJsonObject(QJsonObject &o){
    ControlItemData::writeJsonObject(o);
    o.insert("programm",QString::number(programm->getID().value()));
}
void ProgrammControlItemData::setProgramm(DMX::Programm *p){
    if(p!=programm){
        programm = p;
        emit programmChanged();
    }
}

// Group Model

GroupModel::GroupModel(QAbstractItemModel* deviceModel):deviceModel(deviceModel){
    assert(deviceModel!=nullptr);
    QObject::connect(deviceModel,&QAbstractItemModel::rowsAboutToBeInserted,this,&GroupModel::handleRowsAboutToBeInserted);
    QObject::connect(deviceModel,&QAbstractItemModel::rowsAboutToBeRemoved,this,&GroupModel::handleRowsAboutToBeRemoved);
    QObject::connect(deviceModel,&QAbstractItemModel::rowsInserted,this,&GroupModel::handleRowsInserted);
    QObject::connect(deviceModel,&QAbstractItemModel::rowsRemoved,this,&GroupModel::handleRowsRemoved);
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
GroupControlItemData::GroupControlItemData(const QJsonObject &o, QObject *parent):ControlItemData(o,parent),name(o["name"].toString()){
    const auto enabled = o["enabledDevices"].toArray();
    auto iter = ModelManager::get().getDevices().cbegin();
    int index = 0;
    for(const auto e_ : enabled){
        const auto e = e_.toString().toLongLong();
        const auto start = iter;
        while (iter!=ModelManager::get().getDevices().cend()) {
            if((**iter).getID()==e){
                groupModel.setEnabled(index,true);
                ++iter;
                ++index;
                continue;
            }
            ++iter;
            ++index;
        }
        iter = ModelManager::get().getDevices().cbegin();
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
    auto prog = ModelManager::get().getDevices().cbegin();
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

void GroupControlItemData::forEach(std::function<void (DMX::Device *)> f){
    auto prog = ModelManager::get().getDevices().cbegin();
    auto en = groupModel.getEnabledVector().cbegin();
    for(;en!=groupModel.getEnabledVector().cend();++prog,++en){
        if(*en){
            f(prog->get());
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

SwitchGroupControlItemData::SwitchGroupControlItemData(const QJsonObject &o, QObject *parent):GroupControlItemData(o,parent),
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
        forEach([this](DMX::Device * d){
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
        forEach([=](DMX::Device* d){
            auto filter = d->getFilterForChannelindex(0);
            if(filter)filter->setMaxOperation(o);
        });
        emit maxOperationChanged();
    }
}
void DimmerGroupControlItemData::setMinOperation(Operation o){
    if(minOperation!=o){
        minOperation = o;
        forEach([=](DMX::Device* d){
            auto filter = d->getFilterForChannelindex(0);
            if(filter)filter->setMinOperation(o);
        });
        emit minOperationChanged();
    }
}

void DimmerGroupControlItemData::setMaxValue(unsigned char o){
    if(maxValue!=o){
        maxValue = o;
        forEach([=](DMX::Device* d){
            auto filter = d->getFilterForChannelindex(0);
            if(filter)filter->setMaxValue(o);
        });
        emit maxValueChanged();
    }
}
void DimmerGroupControlItemData::setMinValue(unsigned char o){
    if(minValue!=o){
        minValue = o;
        forEach([=](DMX::Device* d){
            auto filter = d->getFilterForChannelindex(0);
            if(filter)filter->setMinValue(o);
        });
        emit minValueChanged();
    }
}

void DimmerGroupControlItemData::setValue(unsigned char o){
    if(value!=o){
        value = o;
        forEach([=](DMX::Device* d){
            auto filter = d->getFilterForChannelindex(0);
            if(filter)filter->setValue(o);
        });
        emit valueChanged();
    }
}
void DimmerGroupControlItemData::shouldOverrideValue(bool o){
    if(shouldOverrideValue_!=o){
        shouldOverrideValue_=o;
        forEach([=](DMX::Device* d){
            auto filter = d->getFilterForChannelindex(0);
            if(filter)filter->shouldOverrideValue(o);
        });
        emit shouldOverrideValueChanged();
    }
}

// start ProgramBlockControlItemData

ProgramBlockControlItemData::ProgramBlockControlItemData(Modules::ProgramBlock * p,QObject *parent):ControlItemData(ControlItemData::PROGRAM_BLOCK,parent){
    setProgramBlock(p);
}

ProgramBlockControlItemData::ProgramBlockControlItemData(const QJsonObject &o,QObject *parent):
    ControlItemData(o,parent){
    ID id(o);
    for(const auto & p : Modules::ProgramBlockManager::model){
        if(p->getID() == id){
            setProgramBlock(p.get());
            return;
        }
    }
    throw std::runtime_error("No ProgramBlock with id " + std::to_string(id.value()) + " exists.");
}

void ProgramBlockControlItemData::writeJsonObject(QJsonObject &o){
    ControlItemData::writeJsonObject(o);
    if(program)
        o.insert("programBlock",QString::number(program->getID().value()));
}
void ProgramBlockControlItemData::setProgramBlock(Modules::ProgramBlock *p){
    if(p!=program){
        QObject::disconnect(connection);
        program = p;
        if(program){
            QQmlEngine::setObjectOwnership(program,QQmlEngine::CppOwnership);
            connection = QObject::connect(program,&QObject::destroyed,[this](){
                setProgramBlock(nullptr);
            });
        }
        emit programBlockChanged();
    }
}

} // namespace GUI
