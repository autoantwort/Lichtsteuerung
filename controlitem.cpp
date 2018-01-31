#include "controlitem.h"
#include <cmath>

ControlItem::ControlItem()
{    
    setAcceptedMouseButtons(Qt::LeftButton|Qt::RightButton);
    setKeepMouseGrab(true);
    setAcceptHoverEvents(true);
}

void ControlItem::setBlockWidth(int width){
    if(width!=blockWidth){
        blockWidth = width;
        setWidth(width*rasterSize);
        emit blockWidthChanged();
    }
}
void ControlItem::setBlockHeight(int height){
    if(height!=blockHeight){
        blockHeight = height;
        setHeight(height*rasterSize);
        emit blockHeightChanged();
    }
}
void ControlItem::savePositionInControlData(){
    if (data) {
        data->setStartXBlock(x()/rasterSize);
        data->setStartYBlock(y()/rasterSize);
    }
}
void ControlItem::setMoveable(bool m){
    if(m!=moveable){
        moveable=m;
        emit moveableChanged();
    }
}
void ControlItem::setControlData(ControlItemData *c){
    if(data!=c){
        if(c){
            setX(c->getStartXBlock()*rasterSize);
            setY(c->getStartYBlock()*rasterSize);
        }
        data = c;
        emit controlDataChanged();
    }
}

void ControlItem::mousePressEvent(QMouseEvent *event){
    if(event->buttons()&Qt::LeftButton&&moveable){
        event->accept();
        grabPos = event->globalPos();
        startPos.setX(x());
        startPos.setY(y());
    }
}

void ControlItem::mouseMoveEvent(QMouseEvent *event){
    if(event->buttons()&Qt::LeftButton&&moveable){
        event->accept();
        const auto diffPos = grabPos-event->globalPos();
        auto newPos = startPos-diffPos;
        newPos /= rasterSize;
        newPos *= rasterSize;
        if(newPos.x()<0){
            newPos.setX(0);
        }
        if(newPos.x()+width()>(parentItem()->width()/rasterSize)*rasterSize){
            newPos.setX((parentItem()->width()/rasterSize)*rasterSize-width());
        }
        if(newPos.y()<0){
            newPos.setY(0);
        }
        if(newPos.y()+height()>(parentItem()->height()/rasterSize)*rasterSize){
            newPos.setY((parentItem()->height()/rasterSize)*rasterSize-height());
        }
        setProperty("x",newPos.x());
        setProperty("y",newPos.y());
    }
}

void ControlItem::mouseReleaseEvent(QMouseEvent *event){
    event->accept();
    if(event->button()&Qt::LeftButton){

    }else if(event->button()&Qt::RightButton){
        emit openPopup(event->x(),event->y());
    }
}

void ControlItem::itemChange(ItemChange change, const ItemChangeData &value){
    if(change==QQuickItem::ItemChildAddedChange){
        if(childItems().size()<=3)
            return;
        const auto last = childItems().at(childItems().size()-2);
        if(last->x()+last->width()+value.item->width()>width()){
            value.item->setY(last->y()+last->height());
            value.item->setX(0);
        }else{
            value.item->setY(last->y());
            value.item->setX(last->x()+last->width());
        }
    }
    QQuickItem::itemChange(change, value);
}


void ControlItem::hoverEnterEvent(QHoverEvent *event){
    event->accept();
    auto newManLength = std::sqrt(std::pow(width() - event->posF().x(),2) + std::pow(event->posF().y(),2));
    if(newManLength<40)
        emit settingVisibleChange(true);
}
void ControlItem::hoverMoveEvent(QHoverEvent *event){
    event->accept();
    auto oldManLength = std::sqrt(std::pow(width() - event->oldPosF().x(),2) + std::pow(event->oldPosF().y(),2));
    auto newManLength = std::sqrt(std::pow(width() - event->posF().x(),2) + std::pow(event->posF().y(),2));
    if(oldManLength<40&&newManLength>=40)
        emit settingVisibleChange(false);
    else if(oldManLength>40&&newManLength<=40)
        emit settingVisibleChange(true);
}
void ControlItem::hoverLeaveEvent(QHoverEvent *event){
    event->accept();
    emit settingVisibleChange(false);
}

