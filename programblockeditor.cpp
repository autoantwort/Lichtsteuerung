#include "programblockeditor.h"
#include <QQmlEngine>
#include <QPropertyAnimation>

QQmlEngine*  ProgramBlockEditor::engine = nullptr;

ProgramBlockEditor::ProgramBlockEditor():programBlockEntry(engine,QUrl("qrc:/ProgramBlockEntry.qml")),
    programBlockConnection(engine,QUrl("qrc:/ProgramBlockConnection.qml"))
{
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
        component->setParentItem(this);
        QQmlEngine::setObjectOwnership(component,QQmlEngine::JavaScriptOwnership);
        component->setProperty("text",p.get()->getName());
        component->setX(0);
        component->setY(y);
        component->setWidth(p.get()->getOutputLength());
        y += 70;
        components[p.get()] = component;
    }
    for(const auto & p : programBlock->getFilter()){
        QQuickItem*  component = qobject_cast<QQuickItem*>(programBlockEntry.create());
        component->setParentItem(this);
        QQmlEngine::setObjectOwnership(component,QQmlEngine::JavaScriptOwnership);
        component->setProperty("text",dynamic_cast<Named*>(p.second.source.get())->getName());
        component->setX(0);
        component->setY(y);
        component->setWidth(p.second.source.get()->getInputLength());
        y += 70;
        components[dynamic_cast<Named*>(p.second.source.get())] = component;
    }
    for(const auto & p : programBlock->getConsumer()){
        QQuickItem*  component = qobject_cast<QQuickItem*>(programBlockEntry.create());
        component->setParentItem(this);
        QQmlEngine::setObjectOwnership(component,QQmlEngine::JavaScriptOwnership);
        component->setProperty("text",dynamic_cast<Named*>(p.source.get())->getName());
        component->setX(0);
        component->setY(y);
        component->setWidth(p.source.get()->getInputLength());
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

}
