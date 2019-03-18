#include "programblockeditor.h"
#include <QQmlEngine>
#include <QPropertyAnimation>
#include "programms/property.hpp"
#include "errornotifier.h"

QQmlEngine*  ProgramBlockEditor::engine = nullptr;

ProgramBlockEditor::ProgramBlockEditor():programBlockEntry(engine,QUrl("qrc:/ProgramBlockEntry.qml")),
    programBlockConnection(engine,QUrl("qrc:/ProgramBlockConnection.qml"))
{
    setAcceptedMouseButtons(Qt::AllButtons);
    /*QQuickItem *c1 ;
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
    });*/
    /*
    property alias targetStartIndex: path.startX
    property alias targetLength: topRight.relativeX
    property alias sourceStartIndex: bottomLeft.x
    property real sourceLength
    property real targetBaseline
    property real sourceBaseline*/
}
Q_DECLARE_METATYPE(Modules::PropertyBase *)

void ProgramBlockEditor::updatePossibleEntries(){
    QStringList l;
    using namespace Modules;
    for(const auto & m : ModuleManager::singletone()->getProgrammModules()){
        l << QString::fromStdString(m.name()) + "(Program)";
    }
    for(const auto & m : ModuleManager::singletone()->getFilterModules()){
        l << QString::fromStdString(m.name()) + "(Filter)";
    }
    for(const auto & m : ModuleManager::singletone()->getConsumerModules()){
        l << QString::fromStdString(m.name()) + "(Consumer)";
    }
    possibleEntryModel.setStringList(l);
}

void ProgramBlockEditor::updateInputOutputModels(){
    QStringList in, out;
    using namespace Modules;
    for(const auto & p : programBlock->getPrograms()){
        out << QString::fromLatin1(p->getName()) + "(Program)";
    }
    for(const auto & p : programBlock->getFilter()){
        out << QString::fromLatin1(static_cast<Filter*>(p.second.source.get())->getName()) + "(Filter)";
        in << QString::fromLatin1(static_cast<Filter*>(p.second.source.get())->getName()) + "(Filter)";
    }
    for(const auto & p : programBlock->getConsumer()){
        in << QString::fromLatin1(static_cast<Consumer*>(p.source.get())->getName()) + "(Consumer)";
    }
    inputDataConsumerModel.setStringList(in);
    outputDataProducerModel.setStringList(out);
}

Modules::OutputDataProducer * getOutputDataProducer(Modules::ProgramBlock * pb, QString name_){
    if(name_.endsWith("(Program)")){
        auto name = name_.left(name_.length() - 9).toStdString();
        for(const auto &p : pb->getPrograms()){
            if(p->getName() == name)
                return p.get();
        }
    }
    if(name_.endsWith("(Filter)")){
        auto name = name_.left(name_.length() - 8).toStdString();
        for(const auto &p : pb->getFilter()){
            if(static_cast<Modules::Filter*>(p.second.source.get())->getName() == name)
                return static_cast<Modules::Filter*>(p.second.source.get());
        }
    }
    return nullptr;
}

void ProgramBlockEditor::addConnection(int inputIndex, int length, int outputIndex, int startIndex){
    if(inputIndex< 0 || inputIndex >= inputDataConsumerModel.rowCount()){
        return;
    }
    if(outputIndex< 0 || outputIndex >= outputDataProducerModel.rowCount()){
        return;
    }
    using namespace Modules;
    auto inputName = inputDataConsumerModel.data(inputDataConsumerModel.index(inputIndex)).toString();
    auto outputName = outputDataProducerModel.data(outputDataProducerModel.index(outputIndex)).toString();
    if(inputName.endsWith("(Filter)")){
        auto name = inputName.left(inputName.length() - 8).toStdString();
        for(auto & f : programBlock->getFilter()){
            if(static_cast<Filter*>(f.second.source.get())->getName() == name){
                f.second.addTarget(length,getOutputDataProducer(programBlock,outputName),startIndex);
            }
        }
    }else{/*"(Consumer)"*/
        auto name = inputName.left(inputName.length() - 10).toStdString();
        for(auto & f : programBlock->getConsumer()){
            if(static_cast<Filter*>(f.source.get())->getName() == name){
                f.addTarget(length,getOutputDataProducer(programBlock,outputName),startIndex);
            }
        }
    }
    recreateView();
}

void ProgramBlockEditor::addConnection(int length, int startIndex){
    if(dragEndItem == nullptr || length < 0 || startIndex < 0 )
        return;
    using namespace Modules;
    OutputDataProducer * output = dynamic_cast<OutputDataProducer*>(dragStartItem->property("propertyBase").value<PropertyBase*>());
    InputDataConsumer  * input  = dynamic_cast<InputDataConsumer*>(dragEndItem->property("propertyBase").value<PropertyBase*>());
    if(!input||!output)
        return;
    for(auto & i : programBlock->getFilter()){
        if(i.second.source.get() == input){
            i.second.addTarget(length,output,startIndex);
            recreateView();
            return;
        }
    }
    for(auto & i : programBlock->getConsumer()){
        if(i.source.get() == input){
            i.addTarget(length,output,startIndex);
            recreateView();
            return;
        }
    }
}

void ProgramBlockEditor::removeEntry(){
    if(dragStartItem){
        using namespace Modules;
        auto pb = dragStartItem->property("propertyBase").value<PropertyBase*>();
        auto p = dynamic_cast<Program*>(pb);
        auto f = dynamic_cast<Filter*>(pb);
        auto c = dynamic_cast<Consumer*>(pb);
        if(p){
            programBlock->removeProgram(p);
        }else if(f){
            programBlock->removeFilter(f);
        }else if(c){
            programBlock->removeConsumer(c);
        }
        dragStartItem = nullptr;
        recreateView();
    }
}

void ProgramBlockEditor::removeIncomingConnections(){
    if(!dragStartItem)
        return;
    using namespace Modules;
    auto pb = dragStartItem->property("propertyBase").value<PropertyBase*>();
    for(auto & i : programBlock->getFilter()){
        if(static_cast<Filter*>(i.second.source.get())==pb){
            i.second.targeds.clear();
            recreateView();
            return;
        }
    }
    for( auto & i : programBlock->getConsumer()){
        if(static_cast<Consumer*>(i.source.get())==pb){
            i.targeds.clear();
            recreateView();
            return;
        }
    }
}

void ProgramBlockEditor::addEntry(int index, int size){
    if(index < 0 || index >= possibleEntryModel.rowCount()){
        return;
    }
    using namespace Modules;
    QString name = possibleEntryModel.data(possibleEntryModel.index(index)).toString();
    if(name.endsWith("(Program)")){
        auto p = ModuleManager::singletone()->createProgramm(name.left(name.length()-9).toStdString());
        if(p){
            p->setOutputLength(size);
            programBlock->addProgramm(p);
            qDebug() << "added";
            recreateView();
        }else{
            qDebug()<<"Invalid pointer for : "  << name;
        }
    } else if(name.endsWith("(Filter)")){
        auto p = ModuleManager::singletone()->createFilter(name.left(name.length()-8).toStdString());
        if(p){
           p->setInputLength(size);
           programBlock->addFilter(Modules::detail::Connection(p),1);
           qDebug() << "added";
           recreateView();
        }else{
           qDebug()<<"Invalid pointer for : "  << name;
        }
    } else if(name.endsWith("(Consumer)")){
        auto p = ModuleManager::singletone()->createConsumer(name.left(name.length()-10).toStdString());
        if(p){
            p->setInputLength(size);
            programBlock->addConsumer(Modules::detail::Connection(p));
            recreateView();
            qDebug() << "added";
        }else{
            qDebug()<<"Invalid pointer for : "  << name;
        }
    }else{
        qDebug() << "ERROR : " << name;
    }
}

void ProgramBlockEditor::recreateView(){
    for(auto i : this->childItems()){
        if(i->objectName()=="removeable")
        i->setParentItem(nullptr);
    }
    if(programBlockEntry.isError()){
        qDebug() << programBlockEntry.errorString();
        ErrorNotifier::showError(programBlockEntry.errorString());
        for(const auto & e : programBlockEntry.errors()){
            qDebug() << e;
        }
    }if(programBlockConnection.isError()){
        qDebug() << programBlockConnection.errorString();
        ErrorNotifier::showError(programBlockConnection.errorString());
        for(const auto & e : programBlockConnection.errors()){
            qDebug() << e;
        }
    }
    if(!programBlock||!programBlockEntry.isReady()||!programBlockConnection.isReady())
        return;
    using namespace Modules;
    std::map<Named*,QQuickItem*> components;
    int y = 0;
    int x = 0;
    const int LAYER_OFFSET = 15;
    const int GAP_BETWEEN_BLOCKS = 10;
    int layerOffsetX = 0;
    for(const auto & p : programBlock->getPrograms()){
        QQuickItem*  component = qobject_cast<QQuickItem*>(programBlockEntry.create());
        component->setProperty("text",p.get()->getName());
        component->setX(x + layerOffsetX);
        component->setY(y);
        component->setWidth(p.get()->getOutputLength() * scale);
        component->setProperty("propertyBase",QVariant::fromValue(static_cast<PropertyBase*>(p.get())));
        component->setObjectName("removeable");
        x += component->width() + GAP_BETWEEN_BLOCKS;
        components[p.get()] = component;
    }
    x=0;
    int lastLayer = -1;
    for(const auto & p : programBlock->getFilter()){
        if(lastLayer!=p.first){
            x = 0;
            y += spaceBetweenLayers;
            layerOffsetX += LAYER_OFFSET;
        }
        QQuickItem*  component = qobject_cast<QQuickItem*>(programBlockEntry.create());
        component->setProperty("text",dynamic_cast<Named*>(p.second.source.get())->getName());
        component->setX(x + layerOffsetX);
        component->setY(y);
        component->setWidth(p.second.source.get()->getInputLength() * scale);
        component->setProperty("propertyBase",QVariant::fromValue(dynamic_cast<PropertyBase*>(p.second.source.get())));
        component->setObjectName("removeable");
        components[dynamic_cast<Named*>(p.second.source.get())] = component;
        x+= component->width() + GAP_BETWEEN_BLOCKS;
        lastLayer = p.first;
    }
    y+=spaceBetweenLayers;
    layerOffsetX += LAYER_OFFSET;
    x = 0;
    for(const auto & p : programBlock->getConsumer()){
        QQuickItem*  component = qobject_cast<QQuickItem*>(programBlockEntry.create());
        component->setProperty("text",dynamic_cast<Named*>(p.source.get())->getName());
        component->setX(x + layerOffsetX);
        component->setY(y);
        component->setWidth(p.source.get()->getInputLength() * scale);
        component->setProperty("propertyBase",QVariant::fromValue(dynamic_cast<PropertyBase*>(p.source.get())));
        component->setObjectName("removeable");
        x += component->width() + GAP_BETWEEN_BLOCKS;
        components[dynamic_cast<Named*>(p.source.get())] = component;
    }


    for(const auto & p : programBlock->getFilter()){
        int index = 0;
        for(const auto & c : p.second.targeds){
            QQuickItem*  component = qobject_cast<QQuickItem*>(programBlockConnection.create());
            component->setParentItem(this);
            QQmlEngine::setObjectOwnership(component,QQmlEngine::JavaScriptOwnership);            
            component->setProperty("targetLength",c.first*scale);
            component->setProperty("sourceStartIndex",index*scale);
            component->setProperty("sourceLength",c.first*scale);
            const auto elem1 = components[dynamic_cast<Named*>(c.second.targed)];
            component->setProperty("targetBaseline",elem1->y() + elem1->height());
            component->setProperty("targetStartIndex",elem1->x() + c.second.startIndex*scale);
            const auto elem2 = components[dynamic_cast<Named*>(p.second.source.get())];
            component->setProperty("sourceStartIndex",index*scale + elem2->x());
            component->setProperty("sourceBaseline",elem2->y());
            component->setProperty("color",QColor(23,255,23,50));
            component->setObjectName("removeable");

            QObject::connect(elem1,&QQuickItem::xChanged,[=](){
                component->setProperty("targetStartIndex",elem1->x());
            });
            QObject::connect(elem1,&QQuickItem::yChanged,[=](){
                component->setProperty("targetBaseline",elem1->y()+elem1->height());
            });

            QObject::connect(elem2,&QQuickItem::xChanged,[=](){
                component->setProperty("sourceStartIndex",elem2->x() + index*scale);
            });
            QObject::connect(elem2,&QQuickItem::yChanged,[=](){
                component->setProperty("sourceBaseline",elem2->y());
            });
            index += c.first;
        }
    }

    for(const auto & p : programBlock->getConsumer()){
        int index = 0;
        for(const auto & c : p.targeds){
            QQuickItem*  component = qobject_cast<QQuickItem*>(programBlockConnection.create());
            component->setParentItem(this);
            QQmlEngine::setObjectOwnership(component,QQmlEngine::JavaScriptOwnership);
            component->setProperty("targetLength",c.first*scale);
            component->setProperty("sourceStartIndex",index*scale);
            component->setProperty("sourceLength",c.first*scale);
            const auto elem1 = components[dynamic_cast<Named*>(c.second.targed)];
            component->setProperty("targetBaseline",elem1->y() + elem1->height());
            component->setProperty("targetStartIndex",elem1->x() + c.second.startIndex*scale);
            const auto elem2 = components[dynamic_cast<Named*>(p.source.get())];
            component->setProperty("sourceStartIndex",elem2->x()+index*scale);
            component->setProperty("sourceBaseline",elem2->y());
            component->setProperty("color",QColor(23,255,23));
            component->setObjectName("removeable");

            QObject::connect(elem1,&QQuickItem::xChanged,[=](){
                component->setProperty("targetStartIndex",elem1->x()+c.second.startIndex*scale);
            });
            QObject::connect(elem1,&QQuickItem::yChanged,[=](){
                component->setProperty("targetBaseline",elem1->y()+elem1->height());
            });

            QObject::connect(elem2,&QQuickItem::xChanged,[=](){
                component->setProperty("sourceStartIndex",elem2->x() + index*scale);
            });
            QObject::connect(elem2,&QQuickItem::yChanged,[=](){
                component->setProperty("sourceBaseline",elem2->y());
            });

            index += c.first;
        }
    }

    // add components after the shapes, so that the childAt finds the components and not the shapes
    for(const auto & p : components){
        p.second->setParentItem(this);
        QQmlEngine::setObjectOwnership(p.second,QQmlEngine::JavaScriptOwnership);
    }
}

void print(PropertyInformationModel * p){
    for(const auto &o:*p){
        qDebug()<<o->getName() << o->getValue();
    }
}

template<typename TargetType>
void transferData(::detail::PropertyInformation *pi,Modules::Property * p){
    p->asNumeric<TargetType>()->setValue(pi->getValue().value<TargetType>());
}


void detail::PropertyInformation::updateValue(){
    qDebug()<<"set value";
    using namespace Modules;
    if(named){
        auto p = dynamic_cast<Program*>(named);
        auto f = dynamic_cast<Filter*>(named);
        auto c = dynamic_cast<Consumer*>(named);
        if(p){
            p->setOutputLength(value.toInt());
        }else if(f){
            f->setInputLength(value.toInt());
        }else if(c){
            c->setInputLength(value.toInt());
        }
        if(updateCallback)
            updateCallback();
        return;
    }
    switch (getType()) {
        case Property::Double: transferData<double>(this,property);
            break;
        case Property::Float: transferData<float>(this,property);
            break;
        case Property::Int: transferData<int>(this,property);
            break;
        case Property::Long: transferData<long>(this,property);
            break;
        case Property::Bool:
            property->asBool()->setValue(getValue().toBool());
            break;
        case Property::String:
            property->asString()->setValue(getValue().toString().toStdString());
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

QQuickItem * ProgramBlockEditor::getItemWithPropertyBase(QMouseEvent *event){
    auto comp = childAt(event->x(),event->y());
    if(!comp){
        return nullptr;
    }

    while (comp != this && !comp->property("propertyBase").isValid()) {
        comp = comp->parentItem();
    }
    if(comp->property("propertyBase").isValid())
        return comp;
    return nullptr;
}

void ProgramBlockEditor::mousePressEvent(QMouseEvent *event){
    dragStartItem = getItemWithPropertyBase(event);
    if(dragStartItem){
        if(event->modifiers().testFlag(Qt::ShiftModifier)){
            // only filter can be moved
            if(dynamic_cast<Modules::Filter*>(dragStartItem->property("propertyBase").value<Modules::PropertyBase*>())!= nullptr){
                dragType = MovePermanent;
            }
        }else if(event->modifiers().testFlag(Qt::AltModifier)){
            dragType = AddReverseConnection;
#ifdef Q_OS_MAC
        }else if(event->modifiers().testFlag(Qt::MetaModifier)){
#else
        }else if(event->modifiers().testFlag(Qt::ControlModifier)){
#endif
            dragType = MoveTemporarily;
            dragOffsetInItem = event->pos() - dragStartItem->position();
        }else{
            dragType = AddConnection;
        }
    }else{
        dragType = None;
    }
    qDebug() << "pressed "<<event->pos();
    qDebug() << "Type : " << dragType;
}

void ProgramBlockEditor::mouseMoveEvent(QMouseEvent *event){
    //qDebug() << "moved "<<event->pos();
    using namespace Modules;
    if(dragType==MoveTemporarily){
        dragStartItem->setPosition(event->pos()-dragOffsetInItem);
    }else if(dragType == MovePermanent){
        bool larger;
        if((larger = event->y()-dragStartItem->y()>spaceBetweenLayers/2)||dragStartItem->y()-event->y()>spaceBetweenLayers/2){
            //move element to the next layer
            PropertyBase * pb = dragStartItem->property("propertyBase").value<PropertyBase*>();
            if(larger){
                for(auto i = programBlock->getFilter().cbegin();i!=programBlock->getFilter().cend();++i){
                    if(static_cast<Filter*>(i->second.source.get())==static_cast<Filter*>(pb)){
                        const auto saveI = i;
                        Modules::detail::Connection c = i->second;
                        int layer = i->first;
                        ++i;
                        // check if ne should move the component:
                        if(i==programBlock->getFilter().cend() && dragStartItem->x()<=0.5) // we are the last component and alone in a row
                            break;
                        while (i!=programBlock->getFilter().cend() && i->first == layer) { // find the next component in a other layer
                            ++i;
                        }
                        if(i==programBlock->getFilter().cend()){ // we was in the last row with another component
                            layer++;
                        }else{ // we go to the next layer
                            layer = i->first;
                        }
                        programBlock->getFilter().erase(saveI);
                        programBlock->addFilter(std::move(c),layer);
                        break;
                    }
                }
            }else{
                // smaller: same as larger with reserve iterators
                for(auto i = programBlock->getFilter().crbegin();i!=programBlock->getFilter().crend();++i){
                    if(static_cast<Filter*>(i->second.source.get())==static_cast<Filter*>(pb)){
                        Modules::detail::Connection c = i->second;
                        int layer = i->first;
                        ++i;
                        const auto saveI = i;
                        // check if ne should move the component:
                        if(i==programBlock->getFilter().crend() && dragStartItem->x()<=0.5) // we are the last component and alone in a row
                            break;
                        while (i!=programBlock->getFilter().crend() && i->first == layer) { // find the next component in a other layer
                            ++i;
                        }
                        if(i==programBlock->getFilter().crend()){ // we was in the last row with another component
                            layer--;
                        }else{ // we go to the next layer
                            layer = i->first;
                        }
                        programBlock->getFilter().erase(saveI.base());
                        programBlock->addFilter(std::move(c),layer);
                        break;
                    }
                }
            }
            dragStartItem = nullptr;
            recreateView();
            for(const auto & c : childItems()){
                if(c->property("propertyBase").isValid()){
                    if(c->property("propertyBase").value<PropertyBase*>()==pb){
                        dragStartItem = c;
                        break;
                    }
                }
            }
            Q_ASSERT(dragStartItem != nullptr);
        }
    }
}


void ProgramBlockEditor::mouseReleaseEvent(QMouseEvent *event){
    auto comp = getItemWithPropertyBase(event);
    if(dragStartItem == comp && event->button() == Qt::LeftButton){
        // find the item with the right property if exists
        using namespace Modules;
        if(comp){
            print(getPropertyInformationModel());
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
            propertyInformationModel.push_back(new ::detail::PropertyInformation);
            auto s = pb->getProperties().cbegin();
            for(auto t = propertyInformationModel.cbegin() + 1;t!=propertyInformationModel.cend();++t,++s){
                auto & tp = **t;
                auto & sp = **s;
                tp.property = *s;
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
            auto prop = *propertyInformationModel.begin();
            prop->setType(::detail::PropertyInformation::Type::Int);
            prop->setMinValue(1);
            prop->setMaxValue(1000);
            prop->setForwardProperty(false);
            prop->named = pb;
            if(dynamic_cast<InputDataConsumer*>(pb)){
                prop->setName("InputLength");
                prop->setDescription("The legth of the array that the entry consumes");
                prop->setValue(static_cast<int>(dynamic_cast<InputDataConsumer*>(pb)->getInputLength()));
            }else{
                prop->setName("OutputLength");
                prop->setDescription("The legth of the array that the program emits");
                prop->setValue(static_cast<int>(dynamic_cast<OutputDataProducer*>(pb)->getOutputLength()));
            }
            prop->updateCallback = [this,prop](){
                dragStartItem->setWidth(prop->getValue().value<int>()*this->scale);
            };

            print(getPropertyInformationModel());
            setShowProperties(true);

        }else{
            setShowProperties(false);
        }
    }else if(dragStartItem == comp && event->button() == Qt::RightButton && comp && !event->modifiers() && dragType == AddConnection){
        emit openRightClickEntry(event->x(),event->y());
    }
    using namespace Modules;
    if(dragType == AddConnection && comp){
        if(comp->y()>dragStartItem->y()){
            dragEndItem = comp;
            // prüfe, ob überhaupt noch eine Connection hinzugefügt werden kann
            auto pb = dragEndItem->property("propertyBase").value<PropertyBase*>();
            for(const auto & i : programBlock->getFilter()){
                if(static_cast<Filter*>(i.second.source.get())==pb){
                    const auto sum = std::accumulate(i.second.targeds.cbegin(),i.second.targeds.cend(),0,[](int sum,const auto & i){ return sum + i.first;});
                    if(sum >= i.second.source->getInputLength()){
                        dragType = None;
                        return;
                    }
                }
            }
            for(const auto & i : programBlock->getConsumer()){
                if(static_cast<Consumer*>(i.source.get())==pb){
                    const auto sum = std::accumulate(i.targeds.cbegin(),i.targeds.cend(),0,[](int sum,const auto & i){ return sum + i.first;});
                    if(sum >= i.source->getInputLength()){
                        dragType = None;
                        return;
                    }
                }
            }
            askToAddConnection(dragStartItem->property("text").toString(),dragEndItem->property("text").toString());
        }
    }else if(dragType == AddReverseConnection && comp){
        if(comp->y()<dragStartItem->y()){
            dragEndItem = comp;
            // prüfe, ob überhaupt noch eine Connection hinzugefügt werden kann
            auto pb = dragEndItem->property("propertyBase").value<PropertyBase*>();
            for(const auto & i : programBlock->getFilter()){
                if(static_cast<Filter*>(i.second.source.get())==pb){
                    const auto sum = std::accumulate(i.second.targeds.cbegin(),i.second.targeds.cend(),0,[](int sum,const auto & i){ return sum + i.first;});
                    if(sum >= i.second.source->getInputLength()){
                        dragType = None;
                        return;
                    }
                }
            }
            for(const auto & i : programBlock->getConsumer()){
                if(static_cast<Consumer*>(i.source.get())==pb){
                    const auto sum = std::accumulate(i.targeds.cbegin(),i.targeds.cend(),0,[](int sum,const auto & i){ return sum + i.first;});
                    if(sum >= i.source->getInputLength()){
                        dragType = None;
                        return;
                    }
                }
            }
            askToAddConnection(dragStartItem->property("text").toString(),dragEndItem->property("text").toString());
        }
    }
    dragType = None;

}

void ProgramBlockEditor::propertyBaseChanged(Modules::PropertyBase * oldPB, Modules::PropertyBase * newPB){
    for(const auto & child : childItems()){
        auto prop = child->property("propertyBase");
        if(prop.isValid()){
            if(prop.value<Modules::PropertyBase*>() == oldPB){
                child->setProperty("propertyBase",QVariant::fromValue(newPB));
                child->setProperty("text",dynamic_cast<Modules::Named*>(newPB)->getName());
            }
        }
    }
}
