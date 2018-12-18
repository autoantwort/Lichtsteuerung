#include "controlpanel.h"
#include "controlitemdata.h"
#include "controlitem.h"
#include <QJsonArray>
#include <exception>


ControlPanel * ControlPanel::lastCreated = nullptr;
QQmlEngine * ControlPanel::engine = nullptr;

ControlPanel::ControlPanel():programm(engine,QUrl("qrc:/ProgrammControl.qml")),switchGroup(engine,QUrl("qrc:/SwitchGroupControl.qml")),dimmerGroup(engine,QUrl("qrc:/DimmerGroupControl.qml"))
{
    lastCreated = this;
    if(engine==nullptr){
        throw std::runtime_error("Kein Kontext gesetzt!");
    }
    setAcceptHoverEvents(true);
    if(dimmerGroup.isError())
        throw std::runtime_error(dimmerGroup.errorString().toStdString());
    if(switchGroup.isError())
        throw std::runtime_error(switchGroup.errorString().toStdString());
    if(programm.isError())
        throw std::runtime_error(programm.errorString().toStdString());
}

void ControlPanel::writeJsonObject(QJsonObject &o){
    QJsonArray a;
    for(auto i = childItems().cbegin();i!=childItems().cend();++i){
        const auto data = (**i).property("controlData");
        if(data.value<ControlItemData*>() != nullptr){
            static_cast<ControlItem*>(*i)->savePositionInControlData();
            QJsonObject o;
            data.value<ControlItemData*>()->writeJsonObject(o);
            a.append(o);
        }
    }
    o.insert("ControlItems",a);
}

void ControlPanel::loadFromJsonObject(const QJsonObject &o){
    const auto array = o["ControlItems"].toArray();
    for(const auto o_ : array){
        const auto o = o_.toObject();
        if(o["type"].toInt()==ControlItemData::PROGRAMM){
            ControlItem * item = static_cast<ControlItem*>(programm.create());
            item->setControlData(new ProgrammControlItemData(o));
            item->setParentItem(this);
            QQmlEngine::setObjectOwnership(item,QQmlEngine::JavaScriptOwnership);
        }else if(o["type"].toInt()==ControlItemData::SWITCH_GROUP){
            ControlItem * item = static_cast<ControlItem*>(switchGroup.create());
            assert(item!=nullptr);
            item->setControlData(new SwitchGroupControlItemData(o));
            item->setParentItem(this);
            QQmlEngine::setObjectOwnership(item,QQmlEngine::JavaScriptOwnership);
        }else if(o["type"].toInt()==ControlItemData::DIMMER_GROUP){
            ControlItem * item = static_cast<ControlItem*>(dimmerGroup.create());
            assert(item!=nullptr);
            item->setControlData(new DimmerGroupControlItemData(o));
            item->setParentItem(this);
            QQmlEngine::setObjectOwnership(item,QQmlEngine::JavaScriptOwnership);
        }
    }
}

void ControlPanel::addProgrammControl(Programm *p){
    if(p==nullptr)
        throw std::runtime_error("Nullpointer für neues Object");
    QQmlEngine::setObjectOwnership(p,QQmlEngine::CppOwnership);
    ControlItem * item = static_cast<ControlItem*>(programm.create());
    item->setControlData(new ProgrammControlItemData(p,item));
    item->setParentItem(this);
    QQmlEngine::setObjectOwnership(item,QQmlEngine::JavaScriptOwnership);
}

void ControlPanel::addSwitchGroupControl(){
    ControlItem * item = static_cast<ControlItem*>(switchGroup.create());
    item->setControlData(new SwitchGroupControlItemData(item));
    item->setParentItem(this);
    QQmlEngine::setObjectOwnership(item,QQmlEngine::JavaScriptOwnership);
}
void ControlPanel::addDimmerGroupControl(){
    ControlItem * item = static_cast<ControlItem*>(dimmerGroup.create());
    item->setControlData(new DimmerGroupControlItemData(item));
    item->setParentItem(this);
    QQmlEngine::setObjectOwnership(item,QQmlEngine::JavaScriptOwnership);
}


void ControlPanel::hoverEnterEvent(QHoverEvent *event){

}
void ControlPanel::hoverMoveEvent(QHoverEvent *event){
    if(event->oldPos().x()>=width()-menuWidth&&
            event->oldPos().y()>=height()-menuHeight){
        if(event->pos().x()<width()-menuWidth||
                event->pos().y()<height()-menuHeight){
            emit exitMenuArea();
        }
    }
}
void ControlPanel::hoverLeaveEvent(QHoverEvent *event){
    emit exitMenuArea();
}
