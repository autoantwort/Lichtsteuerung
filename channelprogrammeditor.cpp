#include "channelprogrammeditor.h"
#include <cmath>
#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>
#include <QSGVertexColorMaterial>

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
    if(editor->channelProgramm){
        auto cur = editor->currentTimePoint;
        if(cur==editor->channelProgramm->timeline.cend())
            return;
        double min = 0.;
        if(cur!=editor->channelProgramm->timeline.begin()){
            --cur;
            min = cur->time;
            ++cur;
        }
        ++cur;
        double max = 99999.;
        if(cur!=editor->channelProgramm->timeline.cend()){
            max=cur->time;
        }
        time = std::max(min,std::min(max,time));
        --cur;
        if(cur->time != time){
            editor->channelProgramm->changeTimeOfTimePoint(cur->time,time);
            editor->currentTimePoint = editor->channelProgramm->timeline.cend();
            emit hasCurrentChanged();
            emit timeChanged();
            editor->update();
        }


        /*auto cur = editor->getCurrentTimePoint();
        if(cur){
            if(cur->value!=value){
                cur->value = value;
                emit valueChanged();
                editor->update();
            }
        }*/
    }
}

double CurrentTimePointWrapper::getTime()const{
    auto cur = editor->getCurrentTimePoint();
    if(cur)
        return cur->time;
    return 0;
}

void CurrentTimePointWrapper::setCurve(const QEasingCurve::Type &c){
    auto cur = editor->getCurrentTimePoint();
    if(cur){
        if(cur->easingCurveToNextPoint!=c){
            cur->easingCurveToNextPoint.setType(c);
            emit curveChanged();
            editor->update();
        }
    }
}
QEasingCurve::Type CurrentTimePointWrapper::getCurve()const{
    auto cur = editor->getCurrentTimePoint();
    if(cur)
        return cur->easingCurveToNextPoint.type();
    return QEasingCurve::Type::Linear;
}

ChannelProgrammEditor::ChannelProgrammEditor():currentTimePointWrapper(this),modifier(0)
{
    setFlag(ItemHasContents,true);
    setFlag(ItemIsFocusScope,true);
    //setFlag(ItemAcceptsInputMethod,true);
    //setFlag(ItemAcceptsDrops,true);
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    setEnabled(true);
    setFocus(true);
    setActiveFocusOnTab(true);
    setKeepMouseGrab(true);
    setAntialiasing(true);
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
        currentTimePoint = channelProgramm->timeline.cend();
        update();
        emit channelProgrammChanged();
    }
}

void ChannelProgrammEditor::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry){
    updatePolish();
}

void ChannelProgrammEditor::updatePolish(){
    //qDebug()<<"updatePolish\n";
    if (backgroundItem) {
        backgroundItem->setX(0);
        backgroundItem->setY(0);
        backgroundItem->setWidth(width());
        backgroundItem->setHeight(height());
    }

}

QSGNode * ChannelProgrammEditor::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData * nodeData){
    ChannelProgrammEditor::updatePolish();
    QSGTransformNode *node = nullptr;
    QSGGeometryNode *lineNode = nullptr;
    QSGGeometry *lineGeometry = nullptr;
    QSGGeometryNode *pointNode = nullptr;
    QSGGeometry *pointGeometry = nullptr;
    const int segments = 128;
    // effective version: const int count = channelProgramm?channelProgramm->timeline.empty()?0:channelProgramm->timeline.crbegin()->time*16:0;
    const int count = channelProgramm?channelProgramm->timeline.empty()?0:channelProgramm->timeline.begin()->time<0.01?(channelProgramm->timeline.size()-1)*segments:channelProgramm->timeline.size()*segments:0;
    const int pointCount = channelProgramm?channelProgramm->timeline.empty()?0:channelProgramm->timeline.size():0;


    if (!oldNode) {
        node  = new QSGTransformNode;
        lineNode = new QSGGeometryNode;
        lineGeometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), count);
        lineGeometry->setLineWidth(1);
        lineGeometry->setDrawingMode(QSGGeometry::DrawLineStrip);
        lineNode->setGeometry(lineGeometry);
        lineNode->setFlag(QSGNode::OwnsGeometry);


        QSGFlatColorMaterial *material = new QSGFlatColorMaterial;
        material->setColor(graphColor);
        lineNode->setMaterial(material);
        lineNode->setFlag(QSGNode::OwnsMaterial);
        node->appendChildNode(lineNode);


        pointNode = new QSGGeometryNode;
        pointGeometry = new QSGGeometry(QSGGeometry::defaultAttributes_ColoredPoint2D(), pointCount);
        pointGeometry->setLineWidth(5);
        pointGeometry->setDrawingMode(QSGGeometry::DrawPoints);
        pointNode->setGeometry(pointGeometry);
        pointNode->setFlag(QSGNode::OwnsGeometry);

        pointNode->setMaterial(new QSGVertexColorMaterial);
        pointNode->setFlag(QSGNode::OwnsMaterial);

        node->appendChildNode(pointNode);
    } else {
        node = static_cast<QSGTransformNode *>(oldNode);
        lineNode = static_cast<QSGGeometryNode *>(node->childAtIndex(0));
        lineGeometry = lineNode->geometry();
        lineGeometry->allocate(count);
        static_cast<QSGFlatColorMaterial*>(lineNode->material())->setColor(graphColor);
        pointNode = static_cast<QSGGeometryNode *>(node->childAtIndex(1));
        pointGeometry = pointNode->geometry();
        pointGeometry->allocate(pointCount);
    }

    QMatrix4x4 matrix;
    if (modifier.testFlag(X_PRESSED)) {
        xScale+=xDiff*1/32.;
    }else{
    totalXOffset += xDiff;
    if(totalXOffset>0) // nach links beschränken
        totalXOffset=0;
    if(channelProgramm){ // nach rechts beschränken
        if(!channelProgramm->timeline.empty()){
            if (channelProgramm->timeline.crbegin()->time*xScale<-totalXOffset) {
                totalXOffset=-channelProgramm->timeline.crbegin()->time*xScale;
            }
        }
    }}
    matrix.translate(totalXOffset,0);
    matrix.scale(xScale,1.0);
    node->setMatrix(matrix);

    if(count){

        QRectF bounds = boundingRect();
        QSGGeometry::Point2D *vertices = lineGeometry->vertexDataAsPoint2D();

        const qreal stepSize = 1./segments;
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
            for(int i = 0;i<segments;++i,++vertices){
                const auto progress = i / (segments-1.);
                vertices->x = progress * iter->time;
                vertices->y = 255 - (channelProgramm->timeline.crbegin()->easingCurveToNextPoint.valueForProgress(progress) * (to-from)+from);
                vertices->y /= 255.;
                vertices->y *= bounds.height();
            }
        }
        for(;;){
            const auto from = iter->value;
            const auto offsetTime = iter->time;
            const auto &curve = iter->easingCurveToNextPoint;
            ++iter;
            if(iter==channelProgramm->timeline.cend()){
                break;
            }
            const auto to = iter->value;
            const auto timeDiff = iter->time - offsetTime;
            for(int i = 0;i<segments;++i,++vertices){
                const auto progress = i / (segments-1.);
                vertices->x = progress * timeDiff + offsetTime;
                vertices->y = 255 - (curve.valueForProgress(progress) * (to-from)+from);
                vertices->y /= 255.;
                vertices->y *= bounds.height();
            }
        }
        {

            QRectF bounds = boundingRect();
            QSGGeometry::ColoredPoint2D *vertices = pointGeometry->vertexDataAsColoredPoint2D();
            for(auto iter = channelProgramm->timeline.cbegin();iter!=channelProgramm->timeline.cend();++iter){
                vertices->x = iter->time;
                vertices->y = 255 - iter->value;
                vertices->y /= 255.;
                vertices->y *= bounds.height();
                if(iter == currentTimePoint){
                    vertices->r=0;
                    vertices->g=255;
                    vertices->b=0;
                    vertices->a=255;
                }else{
                    if(lastMousePosition!=INVALID_POS){
                        const auto xDiff = (vertices->x-mapFromVisualX(lastMousePosition.x()))*(xScale);
                        const auto yDiff = vertices->y-lastMousePosition.y();
                        auto disstance = (int)std::sqrt(xDiff*xDiff+yDiff*yDiff)*2;
                        if(disstance<255){
                            vertices->r=255-disstance;
                            vertices->g=0;
                            vertices->b=disstance;
                            vertices->a=255;
                        }else{
                            vertices->r=0;
                            vertices->g=0;
                            vertices->b=255;
                            vertices->a=255;
                        }
                    }else{
                        vertices->r=0;
                        vertices->g=0;
                        vertices->b=255;
                        vertices->a=255;
                    }
                }
                ++vertices;
            }
        }


    }

    lineNode->markDirty(QSGNode::DirtyGeometry);
    pointNode->markDirty(QSGNode::DirtyGeometry);

    return node;
}

decltype(ChannelProgramm::timeline)::iterator ChannelProgrammEditor::getTimePointForPosition(int x_, int y_){
    auto x = mapFromVisualX(x_);
    auto y = getScaledY(y_);
    if (channelProgramm->timeline.empty()) {
        return channelProgramm->timeline.cend();
    }
    auto best = channelProgramm->timeline.end();
    auto minDist = clickRadius;
//    auto begin = channelProgramm->timeline.lower_bound(x-clickRadius);
//    auto end   = channelProgramm->timeline.upper_bound(x+clickRadius);
    auto begin = channelProgramm->timeline.cbegin();
    auto end   = channelProgramm->timeline.cend();
    if(begin==channelProgramm->timeline.cend()){
        begin = channelProgramm->timeline.end();
        --begin;
    }
    for(auto i = begin; i != end;++i){        
        const auto xDiff = (i->time-x)*(xScale);
        const auto yDiff = i->value-y;
        auto dist = std::sqrt(xDiff*xDiff+yDiff*yDiff);
        qDebug()<<"dist"<<dist<<'\n'<<'\n';
        if(dist < minDist){
            minDist = dist;
            best = i;
        }
    }
    return best;
}


void ChannelProgrammEditor::mousePressEvent(QMouseEvent *event){
    update();
    lastMousePosition = event->pos();
    forceActiveFocus(Qt::MouseFocusReason);
    event->accept();

    if(!channelProgramm)
        return;
    if((modifier.testFlag(T_PRESSED)||modifier.testFlag(V_PRESSED))&&valueChangeItem){
        valueChangeItem->setVisible(true);
        valueChangeItem->setX(event->x());
        valueChangeItem->setY(event->y());
    }
    if(channelProgramm->timeline.empty()){
        if(modifier.testFlag(N_PRESSED)){
            qDebug()<<"Add time point<\n";
            currentTimePoint = channelProgramm->timeline.insert(TimePoint(mapFromVisualX(event->x()),getScaledY(event->y()),QEasingCurve::Linear)).first;
            update();
        }
        return;
    }
    if(modifier.testFlag(D_PRESSED)){
        qDebug()<<"D is pressed\n";
        auto iter = getTimePointForPosition(event->x(),event->y());
        if(iter != channelProgramm->timeline.end()){
            channelProgramm->timeline.erase(iter);
            update();
        }
        currentTimePoint = channelProgramm->timeline.cend();
        emit currentTimePointWrapper.hasCurrentChanged();
    }else{
        qDebug()<<" N PRESSED : "<<modifier.testFlag(N_PRESSED)<<'\n';
        if(modifier.testFlag(N_PRESSED)){
            qDebug()<<"Add time point?\n";
            if(getTimePointForPosition(event->x(),event->y())==channelProgramm->timeline.end()){
                currentTimePoint = channelProgramm->timeline.insert(TimePoint(mapFromVisualX(event->x()),getScaledY(event->y()),QEasingCurve::Linear)).first;
                update();
            }
        }else{
            currentTimePoint = getTimePointForPosition(event->x(),event->y());
            qDebug()<<"currentTimePoint Changed";
        }
        emit currentTimePointWrapper.hasCurrentChanged();
        emit currentTimePointWrapper.valueChanged();
        emit currentTimePointWrapper.timeChanged();
        emit currentTimePointWrapper.curveChanged();
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
        qDebug()<<"release key : " <<event->text()<<'\n';
    }
}

void ChannelProgrammEditor::keyPressEvent(QKeyEvent *event){
    event->accept();
    switch (event->key()) {
    case Qt::Key::Key_N:
        modifier.setFlag(Modifier::N_PRESSED);
        qDebug()<<"N Pressed Event"<<modifier.testFlag(D_PRESSED)<<modifier.testFlag(N_PRESSED)<<"\n";
        break;
    case Qt::Key::Key_D:
        qDebug()<<"D Pressed Event\n";
        modifier.setFlag(Modifier::D_PRESSED);
        if(haveCurrentTimePoint()){
            channelProgramm->timeline.erase(currentTimePoint);
            currentTimePoint = channelProgramm->timeline.end();
            emit currentTimePointWrapper.hasCurrentChanged();
            update();
        }
        break;
    case Qt::Key::Key_T:
        modifier.setFlag(Modifier::T_PRESSED);
        if(valueChangeItem){
            valueChangeItem->setVisible(true);
        }
        break;
    case Qt::Key::Key_V:
        modifier.setFlag(Modifier::V_PRESSED);
        // TODO
        if(valueChangeItem){
            valueChangeItem->setVisible(true);
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
                emit currentTimePointWrapper.valueChanged();
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

void ChannelProgrammEditor::wheelEvent(QWheelEvent *event){
#ifdef Q_OS_WIN
    xDiff = event->angleDelta().y();
#else
    xDiff = event->pixelDelta().x();
#endif
    event->accept();
    qDebug()<<"wheel : "<<event->pixelDelta()<<' '<<event->angleDelta()<<' '<<event->delta()<<'\n';
    update();
}

 void ChannelProgrammEditor::hoverMoveEvent(QHoverEvent*event){
     lastMousePosition=event->pos();
     update();
     event->accept();
     if((modifier.testFlag(Modifier::V_PRESSED)||modifier.testFlag(Modifier::T_PRESSED))&&valueChangeItem){
         valueChangeItem->setX(event->pos().x()+10);
         valueChangeItem->setY(event->pos().y());
         QString text = "";
         if(modifier.testFlag(Modifier::V_PRESSED)){
             text +=QString::number(channelProgramm->getValueForTime(std::max<int>(0,mapFromVisualX(event->pos().x())))) + "\n";
         }
         if(modifier.testFlag(Modifier::T_PRESSED)){
             text += QString::number(mapFromVisualX(event->pos().x()));
         }
         valueChangeItem->setProperty("text",text);
     }
 }

void ChannelProgrammEditor::mouseMoveEvent(QMouseEvent *event){
    lastMousePosition = event->pos();
    event->accept();
    if((modifier.testFlag(Modifier::V_PRESSED)||modifier.testFlag(Modifier::T_PRESSED))&&valueChangeItem){
        valueChangeItem->setX(event->x()+10);
        valueChangeItem->setY(event->y());
        QString text = "";
        if(modifier.testFlag(Modifier::V_PRESSED)){
            text +=QString::number(channelProgramm->getValueForTime(std::max<int>(0,mapFromVisualX(event->x())))) + "\n";
        }
        if(modifier.testFlag(Modifier::T_PRESSED)){
            text += QString::number(mapFromVisualX(event->x()));
        }
        valueChangeItem->setProperty("text",text);
    }
    if(haveCurrentTimePoint()){
        const auto timeOffset = 0.05;
        auto before = currentTimePoint;
        auto next = currentTimePoint;
        if(before == channelProgramm->timeline.cbegin()){
            // we are at the first time Point
            before = channelProgramm->timeline.cend();
        }
        --before;
        ++next;
        int newX = mapFromVisualX(event->x());
        int newY = std::max(0,std::min(255,getScaledY(event->y())));

        if(currentTimePoint==channelProgramm->timeline.cbegin()){
            if(newX<0){
                newX=0;
            }else if(newX > next->time - timeOffset){
                newX = next->time - timeOffset;
            }
        }else if(next==channelProgramm->timeline.cend()){
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

        /*if (modifier.testFlag(X_PRESSED)&&!modifier.testFlag(Y_PRESSED)) {
            newY = currentTimePoint->value;
        }else if(!modifier.testFlag(X_PRESSED)&&modifier.testFlag(Y_PRESSED)){
            newX = currentTimePoint->time;
        }*/

        if(currentTimePoint->time != newX){
            *const_cast<double*>(&currentTimePoint->time) = newX;
            emit currentTimePointWrapper.timeChanged();
        }

        if(currentTimePoint->value != newY){
            currentTimePoint->value = newY;
            emit currentTimePointWrapper.valueChanged();
        }
        update();
    }
}

void ChannelProgrammEditor::mouseReleaseEvent(QMouseEvent *event){
    event->accept();
    lastMousePosition = INVALID_POS;
    qDebug()<<event->globalPos()<<'\n';

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
