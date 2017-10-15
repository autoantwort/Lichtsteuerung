#include "channelprogrammeditor.h"
#include <cmath>
#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>

CurrentTimePointWrapper::CurrentTimePointWrapper(ChannelProgrammEditor *editor):editor(editor){}

void CurrentTimePointWrapper::setValue(unsigned char value){
    auto cur = editor->getCurrentTimePoint();
    if(cur){
        if(cur->value!=value){
            cur->value = value;
            emit valueChanged();
            editor->update();
        }
    }
}

unsigned char CurrentTimePointWrapper::getValue()const{
    auto cur = editor->getCurrentTimePoint();
    if(cur)
        return cur->value;
    return 0;
}
bool CurrentTimePointWrapper::hasCurrent(){
    return editor->haveCurrentTimePoint();
}
void CurrentTimePointWrapper::setTime(double time){
    /*auto cur = editor->getCurrentTimePoint();
    if(cur){
        if(cur->value!=value){
            cur->value = value;
            emit valueChanged();
            editor->update();
        }
    }*/
}

double CurrentTimePointWrapper::getTime()const{
    auto cur = editor->getCurrentTimePoint();
    if(cur)
        return cur->time;
    return 0;
}

void CurrentTimePointWrapper::setCurve(const QEasingCurve &c){
    auto cur = editor->getCurrentTimePoint();
    if(cur){
        if(cur->easingCurveToNextPoint!=c){
            cur->easingCurveToNextPoint= c;
            emit curveChanged();
            editor->update();
        }
    }
}
QEasingCurve CurrentTimePointWrapper::getCurve()const{
    auto cur = editor->getCurrentTimePoint();
    if(cur)
        return cur->easingCurveToNextPoint;
    return QEasingCurve::Linear;
}

ChannelProgrammEditor::ChannelProgrammEditor():currentTimePointWrapper(this)
{
    setFlag(ItemHasContents,true);
}

void ChannelProgrammEditor::setBackgroundItem(QQuickItem *b){
    if(backgroundItem!=b){
        if(backgroundItem){
            backgroundItem->setParentItem(nullptr);
        }
        backgroundItem=b;
        if(backgroundItem){
            backgroundItem->setX(0);
            backgroundItem->setY(0);
            backgroundItem->setZ(-1);
            backgroundItem->setWidth(width());
            backgroundItem->setHeight(height());
            backgroundItem->setVisible(true);
            backgroundItem->setParentItem(this);
        }
        emit backgroundItemChanged();
    }
}

void ChannelProgrammEditor::setValueChangeItem(QQuickItem *b){
    if (b!=valueChangeItem) {
        if(valueChangeItem){
            b->setParentItem(nullptr);
        }
        valueChangeItem=b;
        if(valueChangeItem){
            valueChangeItem->setVisible(false);
            valueChangeItem->setParentItem(this);
        }
        emit valueChangeItemChanged();
    }
}

void ChannelProgrammEditor::setChannelProgramm(ChannelProgramm *p){
    if(p!=channelProgramm){
        channelProgramm=p;
        update();
        emit channelProgrammChanged();
    }
}

void ChannelProgrammEditor::updatePolish(){
    if (backgroundItem) {
        backgroundItem->setX(0);
        backgroundItem->setY(0);
        backgroundItem->setWidth(width());
        backgroundItem->setHeight(height());
    }
}

QSGNode * ChannelProgrammEditor::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData * nodeData){
    QSGGeometryNode *node = nullptr;
    QSGGeometry *geometry = nullptr;
    // effective version: const int count = channelProgramm?channelProgramm->timeline.empty()?0:channelProgramm->timeline.crbegin()->time*16:0;
    const int count = channelProgramm?channelProgramm->timeline.empty()?0:channelProgramm->timeline.begin()->time<0.01?(channelProgramm->timeline.size()-1)*16:channelProgramm->timeline.size()*16:0;

    auto matrix = nodeData->transformNode->combinedMatrix();
    matrix.scale(xScale,1.0);
    nodeData->transformNode->setCombinedMatrix(matrix);

    if (!oldNode) {
        node = new QSGGeometryNode;
        geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), count);
        geometry->setLineWidth(2);
        geometry->setDrawingMode(QSGGeometry::DrawLineStrip);
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);

        QSGFlatColorMaterial *material = new QSGFlatColorMaterial;
        material->setColor(graphColor);
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);

    } else {
        node = static_cast<QSGGeometryNode *>(oldNode);
        geometry = node->geometry();
        geometry->allocate(count);
        static_cast<QSGFlatColorMaterial*>(node->material())->setColor(graphColor);
    }

    if(count){

        QRectF bounds = boundingRect();
        QSGGeometry::Point2D *vertices = geometry->vertexDataAsPoint2D();

        const qreal stepSize = 1/16;
        auto iter = channelProgramm->timeline.cbegin();
        /*
         * effective version :
         *
        int i=0;
        if(iter->time>0.001){
            const auto from = channelProgramm->timeline.crbegin()->value;
            const auto to = iter->value;
            for(;;++i){
                const auto progress = i*stepSize / iter->time;
                if(progress>0.99){
                    vertices[i].y = to;
                    break;
                }
                vertices[i].y = channelProgramm->timeline.crbegin()->easingCurveToNextPoint.valueForProgress(progress) * (to-from)+from;
            }
        }
        for(;iter!=channelProgramm->timeline.cend()&&i<count;++i) {

        }*/
        if (iter->time>0.01) {
            const auto from = channelProgramm->timeline.crbegin()->value;
            const auto to = iter->value;
            for(int i = 0;i<16;++i,++vertices){
                const auto progress = i / 15.;
                vertices->x = progress * iter->time;
                vertices->y = 255 - (channelProgramm->timeline.crbegin()->easingCurveToNextPoint.valueForProgress(progress) * (to-from)+from);
                vertices->y /= 255.;
                vertices->y /= bounds.height();
            }
        }
        for(;iter!=channelProgramm->timeline.cend();){
            const auto from = iter->value;
            const auto offsetTime = iter->time;
            ++iter;
            const auto to = iter->value;
            const auto timeDiff = iter->time - offsetTime;
            for(int i = 0;i<16;++i,++vertices){
                const auto progress = i / 15.;
                vertices->x = progress * timeDiff + offsetTime;
                vertices->y = 255 - (iter->easingCurveToNextPoint.valueForProgress(progress) * (to-from)+from);
                vertices->y /= 255.;
                vertices->y /= bounds.height();
            }
        }


    }

    node->markDirty(QSGNode::DirtyGeometry);

    return node;
}

decltype(ChannelProgramm::timeline)::iterator ChannelProgrammEditor::getTimePointForPosition(int x_, int y_){
    auto x = getScaledX(x_);
    auto y = getScaledY(y_);
    if (channelProgramm->timeline.empty()) {
        return channelProgramm->timeline.cend();
    }
    auto best = channelProgramm->timeline.end();
    auto minDist = clickRadius * clickRadius;
    auto begin = channelProgramm->timeline.lower_bound(x-clickRadius);
    auto end   = channelProgramm->timeline.upper_bound(x+clickRadius);
    if(begin==channelProgramm->timeline.cend()){
        begin = channelProgramm->timeline.end();
        --begin;
    }
    for(auto i = begin; i != end;++i){
        auto dist = std::fabs((i->value-y) * (i->time - x));
        if(dist < minDist){
            minDist = dist;
            best = i;
        }
    }
    return best;
}


void ChannelProgrammEditor::mousePressEvent(QMouseEvent *event){
    if(!channelProgramm)
        return;
    if(channelProgramm->timeline.empty())
        return;
    if(modifier.testFlag(D_PRESSED)){
        auto iter = getTimePointForPosition(event->x(),event->y());
        if(iter != channelProgramm->timeline.end()){
            channelProgramm->timeline.erase(iter);
        }
        currentTimePoint = channelProgramm->timeline.cend();
        emit currentTimePointWrapper.hasCurrentChanged();
    }else{
        if(modifier.testFlag(N_PRESSED)){
            if(getTimePointForPosition(event->x(),event->y())==channelProgramm->timeline.end()){
                currentTimePoint = channelProgramm->timeline.insert(TimePoint(getScaledX(event->x()),getScaledY(event->y()),QEasingCurve::Linear)).first;
            }
        }else{
            currentTimePoint = getTimePointForPosition(event->x(),event->y());
        }
        emit currentTimePointWrapper.hasCurrent();
        emit currentTimePointWrapper.valueChanged();
        emit currentTimePointWrapper.timeChanged();
        emit currentTimePointWrapper.curveChanged();
    }
    if((modifier.testFlag(T_PRESSED)||modifier.testFlag(V_PRESSED))&&valueChangeItem){
        valueChangeItem->setVisible(true);
        valueChangeItem->setX(event->x());
        valueChangeItem->setY(event->y());
    }
}

void ChannelProgrammEditor::keyReleaseEvent(QKeyEvent *event){
    if(!event->modifiers().testFlag(Qt::ShiftModifier)){
        switch (event->key()) {
        case Qt::Key::Key_N:
            modifier.setFlag(Modifier::N_PRESSED,false);
            break;
        case Qt::Key::Key_D:
            modifier.setFlag(Modifier::D_PRESSED,false);
            break;
        case Qt::Key::Key_T:
            modifier.setFlag(Modifier::T_PRESSED,false);
            break;
        case Qt::Key::Key_V:
            modifier.setFlag(Modifier::V_PRESSED,false);
            break;
        case Qt::Key::Key_X:
            modifier.setFlag(Modifier::X_PRESSED,false);
            break;
        case Qt::Key::Key_Y:
            modifier.setFlag(Modifier::Y_PRESSED,false);
            break;

        }
    }
}

void ChannelProgrammEditor::keyPressEvent(QKeyEvent *event){
    switch (event->key()) {
    case Qt::Key::Key_N:
        modifier.setFlag(Modifier::N_PRESSED);
        break;
    case Qt::Key::Key_D:
        modifier.setFlag(Modifier::D_PRESSED);
        break;
    case Qt::Key::Key_T:
        modifier.setFlag(Modifier::T_PRESSED);
        if(valueChangeItem){
            valueChangeItem->setVisible(false);
        }
        break;
    case Qt::Key::Key_V:
        modifier.setFlag(Modifier::V_PRESSED);
        // TODO
        if(valueChangeItem){
            valueChangeItem->setVisible(false);
        }
        break;
    case Qt::Key::Key_X:
        modifier.setFlag(Modifier::X_PRESSED);
        break;
    case Qt::Key::Key_Y:
        modifier.setFlag(Modifier::Y_PRESSED);
        break;

    }
    if (haveCurrentTimePoint()) {
        switch (event->key()) {
        case Qt::Key::Key_Up:{
            auto oldValue = currentTimePoint->value;
            if (event->modifiers().testFlag(Qt::MetaModifier)) {
                currentTimePoint->value = 255;
            }else if(event->modifiers().testFlag(Qt::ControlModifier)){
                currentTimePoint->value = std::min(255,currentTimePoint->value+25);
            }else if(event->modifiers().testFlag(Qt::ShiftModifier)){
                currentTimePoint->value = std::min(255,currentTimePoint->value+1);
            }else{
                currentTimePoint->value = std::min(255,currentTimePoint->value+5);
            }
            if(oldValue!=currentTimePoint->value){
                update();
                emit currentTimePointWrapper.valueChanged();
            }
        }
            break;
        case Qt::Key::Key_Down:{
            auto oldValue = currentTimePoint->value;
            if (event->modifiers().testFlag(Qt::MetaModifier)) {
                currentTimePoint->value = 0;
            }else if(event->modifiers().testFlag(Qt::ControlModifier)){
                currentTimePoint->value = std::max(0,currentTimePoint->value-25);
            }else if(event->modifiers().testFlag(Qt::ShiftModifier)){
                currentTimePoint->value = std::max(0,currentTimePoint->value-1);
            }else{
                currentTimePoint->value = std::max(0,currentTimePoint->value-5);
            }
            if(oldValue!=currentTimePoint->value){
                update();
                emit currentTimePointWrapper.timeChanged();
            }
        }
            break;
        case Qt::Key::Key_Left:
            //TODO
            break;
        case Qt::Key::Key_Right:
            //TODO
            break;
        }
    }
}

void ChannelProgrammEditor::mouseMoveEvent(QMouseEvent *event){
    if((modifier.testFlag(Modifier::V_PRESSED)||modifier.testFlag(Modifier::T_PRESSED))&&valueChangeItem){
        valueChangeItem->setX(event->x()+10);
        valueChangeItem->setY(event->y());
        QString text = "";
        if(modifier.testFlag(Modifier::V_PRESSED)){
            text +="value\n";
        }
        if(modifier.testFlag(Modifier::T_PRESSED)){
            text +="time";
        }
        valueChangeItem->setProperty("text",text);
    }
    if(haveCurrentTimePoint()){
        const auto timeOffset = 0.05;
        auto before = currentTimePoint;
        auto next = currentTimePoint;
        --before;
        ++next;
        int newX = getScaledX(event->x());
        int newY = getScaledY(event->y());

        if(currentTimePoint==channelProgramm->timeline.cbegin()){
            if(newX<0){
                newX=0;
            }else if(newX > next->time - timeOffset){
                newX = next->time - timeOffset;
            }
        }else if(currentTimePoint==channelProgramm->timeline.cend()--){
            if(newX< before->time+timeOffset){
                newX=before->time+timeOffset;
            }
        }else{
            if(newX< before->time+timeOffset){
                newX=before->time+timeOffset;
            }else if(newX > next->time - timeOffset){
                newX = next->time - timeOffset;
            }
        }

        if (modifier.testFlag(X_PRESSED)&&!modifier.testFlag(Y_PRESSED)) {
            newY = currentTimePoint->value;
        }else if(!modifier.testFlag(X_PRESSED)&&modifier.testFlag(Y_PRESSED)){
            newX = currentTimePoint->time;
        }
        if(currentTimePoint->value != newY){
            *const_cast<double*>(&currentTimePoint->time) = newX;
            emit currentTimePointWrapper.timeChanged();
        }

        if(currentTimePoint->value != newY){
            currentTimePoint->value = newY;
            emit currentTimePointWrapper.valueChanged();
        }
    }
}

void ChannelProgrammEditor::mouseReleaseEvent(QMouseEvent *event){

}

TimePoint * ChannelProgrammEditor::getCurrentTimePoint(){
    if(haveCurrentTimePoint()){
        return const_cast<TimePoint*>(&*currentTimePoint);
    }
    return nullptr;
}

QString ChannelProgrammEditor::getTooltipText()const{
    return "Hold N and click to create a new TimePoint-\nHold D and click to delete a Timepoint.\nUse dragging to move the time points.";
}
