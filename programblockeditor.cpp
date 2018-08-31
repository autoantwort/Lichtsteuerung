#include "programblockeditor.h"
#include <QQmlEngine>
#include <QPropertyAnimation>
#include "programms/property.hpp"

QQmlEngine*  ProgramBlockEditor::engine = nullptr;

ProgramBlockEditor::ProgramBlockEditor():programBlockEntry(engine,QUrl("qrc:/ProgramBlockEntry.qml")),
    programBlockConnection(engine,QUrl("qrc:/ProgramBlockConnection.qml"))
{
    setAcceptedMouseButtons(Qt::AllButtons);
    QQuickItem *c1 ;
    if(programBlockEntry.isReady()){
        QQuickItem*  component = qobject_cast<QQuickItem*>(programBlockEntry.create());
        component->setParentItem(this);
        QQmlEngine::setObjectOwnership(component,QQmlEngine::JavaScriptOwnership);
        component->setProperty("text","testTest");
        component->setX(5);
        component->setY(20);
        component->setWidth(100);
        c1 = component;


    }else{
        qDebug() << " Component not loaded!";
    }

    QQuickItem*  component = qobject_cast<QQuickItem*>(programBlockConnection.create());
    component->setParentItem(this);
    QQmlEngine::setObjectOwnership(component,QQmlEngine::JavaScriptOwnership);
    component->setProperty("targetStartIndex",0);
    component->setProperty("targetLength",50);
    component->setProperty("sourceStartIndex",10);
    component->setProperty("sourceLength",50);
    component->setProperty("targetBaseline",100);
    component->setProperty("sourceBaseline",160);
    component->setProperty("color",QColor(23,255,23));{
        QPropertyAnimation  *animation = new QPropertyAnimation(component, "sourceStartIndex");
        animation->setDuration(1000);
        animation->setStartValue(10);
        animation->setEndValue(800);
        animation->setLoopCount(-1);
        animation->setEasingCurve(QEasingCurve::OutCubic);
        animation->setParent(this);
        animation->start();}
    {
        QPropertyAnimation  *animation = new QPropertyAnimation(component, "sourceBaseline");
        animation->setDuration(1000);
        animation->setStartValue(160);
        animation->setEndValue(800);
        animation->setLoopCount(-1);
        animation->setEasingCurve(QEasingCurve::OutCubic);
        animation->setParent(this);
        animation->start();}

    QObject::connect(c1,&QQuickItem::xChanged,[=](){
        component->setProperty("targetStartIndex",c1->x());
    });
    QObject::connect(c1,&QQuickItem::yChanged,[=](){
        component->setProperty("targetBaseline",c1->y()+c1->height());
    });
    /*
    property alias targetStartIndex: path.startX
    property alias targetLength: topRight.relativeX
    property alias sourceStartIndex: bottomLeft.x
    property real sourceLength
    property real targetBaseline
    property real sourceBaseline*/
}
Q_DECLARE_METATYPE(Modules::PropertyBase *)

void ProgramBlockEditor::recreateView(){
    for(auto i : this->childItems()){
        i->setParentItem(nullptr);
    }
    if(!programBlock)
        return;
    using namespace Modules;
    std::map<Named*,QQuickItem*> components;
    int y = 0;
    for(const auto & p : programBlock->getPrograms()){
        QQuickItem*  component = qobject_cast<QQuickItem*>(programBlockEntry.create());
        component->setProperty("text",p.get()->getName());
        component->setX(0);
        component->setY(y);
        component->setWidth(p.get()->getOutputLength());
        component->setProperty("propertyBase",QVariant::fromValue(static_cast<PropertyBase*>(p.get())));
        y += 70;
        components[p.get()] = component;
    }
    for(const auto & p : programBlock->getFilter()){
        QQuickItem*  component = qobject_cast<QQuickItem*>(programBlockEntry.create());
        component->setProperty("text",dynamic_cast<Named*>(p.second.source.get())->getName());
        component->setX(0);
        component->setY(y);
        component->setWidth(p.second.source.get()->getInputLength());
        component->setProperty("propertyBase",QVariant::fromValue(dynamic_cast<PropertyBase*>(p.second.source.get())));
        y += 70;
        components[dynamic_cast<Named*>(p.second.source.get())] = component;
    }
    for(const auto & p : programBlock->getConsumer()){
        QQuickItem*  component = qobject_cast<QQuickItem*>(programBlockEntry.create());
        component->setProperty("text",dynamic_cast<Named*>(p.source.get())->getName());
        component->setX(0);
        component->setY(y);
        component->setWidth(p.source.get()->getInputLength());
        component->setProperty("propertyBase",QVariant::fromValue(dynamic_cast<PropertyBase*>(p.source.get())));
        y += 70;
        components[dynamic_cast<Named*>(p.source.get())] = component;
    }


    for(const auto & p : programBlock->getFilter()){
        int index = 0;
        for(const auto & c : p.second.targeds){
            QQuickItem*  component = qobject_cast<QQuickItem*>(programBlockConnection.create());
            component->setParentItem(this);
            QQmlEngine::setObjectOwnership(component,QQmlEngine::JavaScriptOwnership);
            component->setProperty("targetStartIndex",c.second.startIndex);
            component->setProperty("targetLength",c.first);
            component->setProperty("sourceStartIndex",index);
            component->setProperty("sourceLength",c.first);
            const auto elem1 = components[dynamic_cast<Named*>(c.second.targed)];
            component->setProperty("targetBaseline",elem1->y() + elem1->height());
            const auto elem2 = components[dynamic_cast<Named*>(p.second.source.get())];
            component->setProperty("sourceBaseline",elem2->y());
            component->setProperty("color",QColor(23,255,23));
            index += c.first;

            QObject::connect(elem1,&QQuickItem::xChanged,[=](){
                component->setProperty("targetStartIndex",elem1->x());
            });
            QObject::connect(elem1,&QQuickItem::yChanged,[=](){
                component->setProperty("targetBaseline",elem1->y()+elem1->height());
            });

            QObject::connect(elem2,&QQuickItem::xChanged,[=](){
                component->setProperty("sourceStartIndex",elem2->x());
            });
            QObject::connect(elem2,&QQuickItem::yChanged,[=](){
                component->setProperty("sourceBaseline",elem2->y());
            });
        }
    }

    for(const auto & p : programBlock->getConsumer()){
        int index = 0;
        for(const auto & c : p.targeds){
            QQuickItem*  component = qobject_cast<QQuickItem*>(programBlockConnection.create());
            component->setParentItem(this);
            QQmlEngine::setObjectOwnership(component,QQmlEngine::JavaScriptOwnership);
            component->setProperty("targetStartIndex",c.second.startIndex);
            component->setProperty("targetLength",c.first);
            component->setProperty("sourceStartIndex",index);
            component->setProperty("sourceLength",c.first);
            const auto elem1 = components[dynamic_cast<Named*>(c.second.targed)];
            component->setProperty("targetBaseline",elem1->y() + elem1->height());
            const auto elem2 = components[dynamic_cast<Named*>(p.source.get())];
            component->setProperty("sourceBaseline",elem2->y());
            component->setProperty("color",QColor(23,255,23));
            index += c.first;

            QObject::connect(elem1,&QQuickItem::xChanged,[=](){
                component->setProperty("targetStartIndex",elem1->x());
            });
            QObject::connect(elem1,&QQuickItem::yChanged,[=](){
                component->setProperty("targetBaseline",elem1->y()+elem1->height());
            });

            QObject::connect(elem2,&QQuickItem::xChanged,[=](){
                component->setProperty("sourceStartIndex",elem2->x());
            });
            QObject::connect(elem2,&QQuickItem::yChanged,[=](){
                component->setProperty("sourceBaseline",elem2->y());
            });
        }
    }

    // add components after the shapes, so that the childAt finds the components and not the shapes
    for(const auto & p : components){
        p.second->setParentItem(this);
        QQmlEngine::setObjectOwnership(p.second,QQmlEngine::JavaScriptOwnership);
    }
}

template<typename SourceType>
void transferData(Modules::Property & p, ::detail::PropertyInformation &pi){
    pi.setValue(p.asNumeric<SourceType>()->getValue());
    pi.setMinValue(p.asNumeric<SourceType>()->getMin());
    pi.setMaxValue(p.asNumeric<SourceType>()->getMax());
}
template<>
void transferData<long>(Modules::Property & p, ::detail::PropertyInformation &pi){
    pi.setValue(static_cast<qlonglong>(p.asNumeric<long>()->getValue()));
    pi.setMinValue(static_cast<qlonglong>(p.asNumeric<long>()->getMin()));
    pi.setMaxValue(static_cast<qlonglong>(p.asNumeric<long>()->getMax()));
}

void ProgramBlockEditor::mouseReleaseEvent(QMouseEvent *event){
    // find the item with the right property if exists
    using namespace Modules;
    auto comp = childAt(event->x(),event->y());
    if(!comp){
        return;
    }

    while (comp != this && !comp->property("propertyBase").isValid()) {
        comp = comp->parentItem();
    }
    if(comp->property("propertyBase").isValid()){
        Modules::PropertyBase * pb = comp->property("propertyBase").value<Modules::PropertyBase*>();
        if(propertyInformationModel.size()>pb->getProperties().size()){
            for(auto i = propertyInformationModel.cbegin()+pb->getProperties().size();i!=propertyInformationModel.cend();++i){
                delete *i;
            }
            propertyInformationModel.erase(propertyInformationModel.cbegin()+pb->getProperties().size(),propertyInformationModel.cend());
        }else{
            while(propertyInformationModel.size() < pb->getProperties().size()){
                propertyInformationModel.push_back(new ::detail::PropertyInformation);
            }
        }
        auto s = pb->getProperties().cbegin();
        for(auto t = propertyInformationModel.cbegin();t!=propertyInformationModel.cend();++t,++s){
            auto & tp = **t;
            auto & sp = **s;
            tp.setDescription(QString::fromStdString(sp.getDescription()));
            tp.setName(QString::fromStdString(sp.getName()));
            tp.setType(sp.type);
            switch (sp.type) {
                case Property::Double: transferData<double>(sp,tp);
                    break;
                case Property::Float: transferData<float>(sp,tp);
                    break;
                case Property::Int: transferData<int>(sp,tp);
                    break;
                case Property::Long: transferData<long>(sp,tp);
                    break;
                case Property::Bool:
                    tp.setMinValue(0);
                    tp.setMaxValue(1);
                    tp.setValue(sp.asBool()->getValue());
                    break;
                case Property::String:
                    tp.setValue(QString::fromStdString(sp.asString()->getString()));
            }

        }
        setShowProperties(true);

    }else{
        setShowProperties(false);
    }

}
void ProgramBlockEditor::mouseMoveEvent(QMouseEvent *event){
    qDebug() << "moved "<<event->pos();
}void ProgramBlockEditor::mousePressEvent(QMouseEvent *event){

    qDebug() << "pressed "<<event->pos();
}
