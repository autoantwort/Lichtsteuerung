#include "controlpanel.h"
#include "controlitemdata.h"
#include "controlitem.h"
#include <QJsonArray>
#include <exception>
#include <QQmlContext>


ControlPanel * ControlPanel::lastCreated = nullptr;
QQmlEngine * ControlPanel::engine = nullptr;

ControlPanel::ControlPanel():programm(engine,QUrl("qrc:/ControlPane/ProgrammControl.qml")),switchGroup(engine,QUrl("qrc:/ControlPane/SwitchGroupControl.qml")),dimmerGroup(engine,QUrl("qrc:/ControlPane/DimmerGroupControl.qml")),programBlock(engine,QUrl("qrc:/ControlPane/ProgramBlockControl.qml"))
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
    if(programBlock.isError())
        throw std::runtime_error(programBlock.errorString().toStdString());
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

void ControlPanel::createControlItem(QQmlComponent &component, ControlItemData *data){
    QQmlContext* context = new QQmlContext(engine->rootContext(),engine);
    context->setContextProperty("__controlData",data);
    ControlItem * item = static_cast<ControlItem*>(component.create(context));
    item->setParentItem(this);
    data->setParent(item);
    QQmlEngine::setObjectOwnership(item,QQmlEngine::JavaScriptOwnership);
}

void ControlPanel::loadFromJsonObject(const QJsonObject &o){
    const auto array = o["ControlItems"].toArray();
    for(const auto o_ : array){
        const auto o = o_.toObject();
        if(o["type"].toInt()==ControlItemData::PROGRAMM){
            createControlItem(programm,new ProgrammControlItemData(o));
        }else if(o["type"].toInt()==ControlItemData::SWITCH_GROUP){
            createControlItem(switchGroup,new SwitchGroupControlItemData(o));
        }else if(o["type"].toInt()==ControlItemData::DIMMER_GROUP){
            createControlItem(dimmerGroup,new DimmerGroupControlItemData(o));
        }else if(o["type"].toInt()==ControlItemData::PROGRAM_BLOCK){
            createControlItem(programBlock,new ProgramBlockControlItemData(o));
        }
    }
}

void ControlPanel::addProgrammControl(Programm *p){
    if(p==nullptr)
        throw std::runtime_error("Nullpointer für neues Object");
    createControlItem(programm,new ProgrammControlItemData(p));
}

void ControlPanel::addSwitchGroupControl(){
    createControlItem(switchGroup,new SwitchGroupControlItemData());
}
void ControlPanel::addDimmerGroupControl(){
    createControlItem(dimmerGroup,new DimmerGroupControlItemData());
}

void ControlPanel::addProgramBlockControl(Modules::ProgramBlock *p){
    if(p==nullptr)
        throw std::runtime_error("Nullpointer für neues Object");
    createControlItem(programBlock,new ProgramBlockControlItemData(p));
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
