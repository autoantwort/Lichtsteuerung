#include "mapview.h"
#include <QJsonArray>

namespace GUI {


std::pair<Polygon *,Polygon *> generateBar(){
    auto p = new Polygon("Stonework");
    p->setColor(QColor(234,56,1));
    auto s = new Polygon("surfaces");
    s->setColor(QColor(34,56,231));
    auto addRightFixTop = [=](Polygon::Point::number_type startX, Polygon::Point::number_type startY){
        return [=](Polygon::Point::number_type width, Polygon::Point::number_type height)mutable{
            p->addRectangle(startX,startX+width,startY,startY+height);
            startX += width;
        };
    };
    auto addBottomFixRight = [=](Polygon::Point::number_type startX, Polygon::Point::number_type startY){
        return [=](Polygon::Point::number_type height, Polygon::Point::number_type width)mutable{
            p->addRectangle(startX,startX-width,startY,startY+height);
            startY += height;
        };
    };
    auto addBottomFixLeft = [=](Polygon::Point::number_type startX, Polygon::Point::number_type startY){
        return [=](Polygon::Point::number_type height, Polygon::Point::number_type width)mutable{
            p->addRectangle(startX,startX+width,startY,startY+height);
            startY += height;
        };
    };
//    p->addRectangle(left X,right X, top Y, bottom Y);
    //linke wand
    p->addRectangle(-30,0,-30,131);
    p->addRectangle(-30,-19,131,131+110);
    p->addRectangle(-30,0,131+110,131+110+41);
    p->addRectangle(-5,2,131+110+41,131+110+41+112);//tür
    p->addRectangle(-30,0,131+110+41+112,131+110+41+112+116+45+46+25);
    p->addRectangle(0,60,131+110+41+112+116,131+110+41+112+116+45);//block vor box
    // Cocktailwand zum Hauptraum:
    auto y = 131+110+41+112+116+45+46;
    p->addRectangle(0,129,y,y+25);
    p->addRectangle(129+105,129+105+142,y,y+25);
    //wand hinter der Biertheke
    auto adder = addRightFixTop(0,-30);
    adder(314+50,30);
    adder(8,30+30);
    adder(13,30+30+55);
    adder(41,30+30+55+64);
    adder(93,30+30+55+64-26);
    adder(31,30+30+55+64);
    adder(26,30+30+55+64+8);

    s->addRectangle(0,390,0,30);
    s->addRectangle(0,314,105,105+60);
    s->addRectangle(0,314,105+60,105+60+30);

    s->addRectangle(0,60,555,555+46);

    const auto WALL_WIDTH = 30;
    const auto RIGHT_WALL_X = 129+105+142-11+65+94+31;
    //Wand Rechts
    auto rightWall = addBottomFixRight(RIGHT_WALL_X+WALL_WIDTH,30+55+64+8+68);
    rightWall(15,WALL_WIDTH);
    rightWall(44,WALL_WIDTH + 10);
    rightWall(68,WALL_WIDTH);
    rightWall(93,WALL_WIDTH-20);
    rightWall(62,WALL_WIDTH);
    rightWall(13,WALL_WIDTH+31);
    rightWall(33,WALL_WIDTH + 11);
    rightWall(225,WALL_WIDTH + 0);
    rightWall(45,WALL_WIDTH + 11);
    rightWall(150,WALL_WIDTH + 0);
    rightWall(75,WALL_WIDTH - 20);
    rightWall(WALL_WIDTH,WALL_WIDTH + 560);

    //Pfeiler eingang
    const auto PFEILER_Y = 30+55+64+92;
    p->addRectangle(RIGHT_WALL_X-10-135-44,RIGHT_WALL_X-10-135,PFEILER_Y,PFEILER_Y + 44);

    const auto WAND_MITTE_X = 129+105+142-11;
    const auto WAND_MITTE_Y = 30+55+64+8+68+15+44+68+93+62;

    p->addRectangle(WAND_MITTE_X,WAND_MITTE_X+65-31,WAND_MITTE_Y,WAND_MITTE_Y+45);
    p->addRectangle(WAND_MITTE_X+11,WAND_MITTE_X+65-31,WAND_MITTE_Y,WAND_MITTE_Y+13+33+225+45+150);//komplette wand
    p->addRectangle(WAND_MITTE_X+65,WAND_MITTE_X+65-31,WAND_MITTE_Y,WAND_MITTE_Y+13);
    p->addRectangle(WAND_MITTE_X+65-20,WAND_MITTE_X+65-31,WAND_MITTE_Y+33+13,WAND_MITTE_Y+13);

    p->addRectangle(WAND_MITTE_X,WAND_MITTE_X+45,WAND_MITTE_Y+13+33+225,WAND_MITTE_Y+13+33+225+45);//Pfeiler cocktail|dartgang

    const auto COCKTAIL_ROOM_Y = 131+110+41+112+116+45+46+25;
    auto leftCocktailWall = addBottomFixLeft(-WALL_WIDTH,COCKTAIL_ROOM_Y);
    leftCocktailWall(50,WALL_WIDTH);
    leftCocktailWall(110,WALL_WIDTH-10);
    leftCocktailWall(40,WALL_WIDTH);
    leftCocktailWall(110,WALL_WIDTH-10);
    leftCocktailWall(18,WALL_WIDTH);
    leftCocktailWall(19/*wahrscheinlich*/,WALL_WIDTH);
    leftCocktailWall(75,WALL_WIDTH);
    leftCocktailWall(WALL_WIDTH,WALL_WIDTH);

    const auto COCKTAIL_ROOM_HEIGHT = 18 + 110 + 40 + 110 + 50;
    const auto BOTTOM_COCKTAIL_WALL_WIDTH = - 25 - 43 - 45 + 13 + 33 + 225 + 45 + 150 - COCKTAIL_ROOM_HEIGHT - 6/*korrektur*/;

    auto bottomCocktailWall = addRightFixTop(0,COCKTAIL_ROOM_Y+COCKTAIL_ROOM_HEIGHT);
    bottomCocktailWall(221,BOTTOM_COCKTAIL_WALL_WIDTH);
    bottomCocktailWall(47,0);//durchreiche
    s->addRectangle(221,221+47,COCKTAIL_ROOM_Y+COCKTAIL_ROOM_HEIGHT,COCKTAIL_ROOM_Y+COCKTAIL_ROOM_HEIGHT+BOTTOM_COCKTAIL_WALL_WIDTH);
    bottomCocktailWall(109,BOTTOM_COCKTAIL_WALL_WIDTH);

    return {p,s};
}

MapView * MapView::lastCreated = nullptr;

MapView::MapView()
{
    setKeepMouseGrab(true);
    setAcceptedMouseButtons(Qt::LeftButton);

    lastCreated = this;
    auto p = generateBar();
    p.first->setParentItem(this);
    p.first->setParent(this);
    p.second->setParentItem(this);
    p.second->setParent(this);
    /*
    stonework = new Polygon("stonework");
    stonework->setParentItem(this);

    surfaces = new Polygon("surfaces");
    surfaces->setParentItem(this);
    surfaces->setColor(QColor(255, 153, 0));
    surfaces->addRectangle(200,250,10,40);

    stonework->addPoint(Polygon::Point(1.f,2.f));
    stonework->addPoint(Polygon::Point(50.f,7.f));
    stonework->addPoint(Polygon::Point(4.f,80.f));
    stonework->addPoint(Polygon::Point(80.f,100.f));

    stonework->addTriangle(0,1,2);
    stonework->addTriangle(1,2,3);
    stonework->setColor(QColor(0,128,255));
    //stonework->addRectangle(100,200,200,300);

    stonework->addArc(200,170,200,200,230,200);
    stonework->addArc(230,200,200,200,200,230);
    //stonework->addArc(70,200,100,200,100,170);*/


    auto minX = std::numeric_limits<float>::max();
    auto maxX = std::numeric_limits<float>::min();
    auto minY = std::numeric_limits<float>::max();
    auto maxY = std::numeric_limits<float>::min();
    for(const auto &point : p.first->getPoints()){
        minX = std::min(minX,point.x);
        maxX = std::max(maxX,point.x);
        minY = std::min(minY,point.y);
        maxY = std::max(maxY,point.y);
    }
    setImplicitWidth(maxX-minX);
    setImplicitHeight(maxY-minY);
    p.first->translatePoints(-minX,-minY);
    p.second->translatePoints(-minX,-minY);
    translation = {-minX, -minY};
    emit translationChanged();

}




void MapView::loadFromJsonObject(const QJsonObject &o){
    return; //TODO

    auto a = o["polygons"].toArray();
    for(const auto &p_ : a){
        auto poly = new Polygon(p_.toObject());
        poly->setParentItem(this);
        polygons.push_back(poly);
    }
}
void MapView::writeJsonObject(QJsonObject &o) const{
    QJsonArray a;
    for(const auto & p : polygons){
        QJsonObject o ;
        p->writeJsonObject(o);
        a.append(o);
    }
    o.insert("polygons",a);
}

void MapView::mousePressEvent(QMouseEvent *event){
    if(event->modifiers().testFlag(Qt::AltModifier) || event->modifiers().testFlag(Qt::ShiftModifier)){
        event->accept();
    }else{
        // compute distance to controlPoint:
        if(std::sqrt((event->pos() - controlPoint).manhattanLength())<9){
            event->accept();
        }else{
            event->ignore();
        }
    }
}

void MapView::mouseMoveEvent(QMouseEvent *event){
    event->accept();
    setControlPoint(event->pos());
}

}



