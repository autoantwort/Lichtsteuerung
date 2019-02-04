#include "polygon.h"
#include <assert.h>
#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>
#include <algorithm>
#include <QJsonArray>
#include <cstring>
#ifdef _MSC_VER
#include <corecrt_math_defines.h>
#endif

namespace GUI {

Polygon::Polygon(QString name):name(name)
{
    setFlag(ItemHasContents);
}


Polygon::Polygon(const QJsonObject &o):Polygon(o["name"].toString()){

    auto colorString = o["color"].toString();
    if(colorString.startsWith('#')){
        while(colorString.length()<9){
            colorString.append('f');
        }
        //       012345678
        //Have : #RRGGBBAA //css like
        //Need : AARRGGBB  //Qt like
        colorString.remove(0,1);
        colorString.prepend(colorString.at(7));
        colorString.prepend(colorString.at(7));
        colorString.remove(8,2);
        polygonColor.setRgba(colorString.toUInt(nullptr,16));
    }else{
        polygonColor = QColor(colorString);
    }
    {
        auto array = o["points"].toArray();
        for(const auto p_ : array){
            auto p = p_.toObject();
            addPoint(Point(p));
        }
    }
    {
        auto array = o["triangles"].toArray();
        for(const auto p_ : array){
            auto p = p_.toObject();
            triangles.emplace_back(p);
        }
    }
    {
        auto array = o["arcs"].toArray();
        for(const auto p_ : array){
            auto p = p_.toObject();
            arcs.emplace_back(p);
        }
    }
}

void Polygon::writeJsonObject(QJsonObject &o) const{
    {
        QJsonArray a;
        for(const auto p : points){
            QJsonObject o_;
            p.writeJsonObject(o_);
            a.append(o_);
        }
        o.insert("points",a);
    } {
        QJsonArray a;
        for(const auto p : triangles){
            QJsonObject o_;
            p.writeJsonObject(o_);
            a.append(o_);
        }
        o.insert("triangles",a);
    } {
        QJsonArray a;
        for(const auto p : arcs){
            QJsonObject o_;
            p.writeJsonObject(o_);
            a.append(o_);
        }
        o.insert("arcs",a);
    }
    o.insert("name",name);
    QString n;
    polygonColor.rgb();
    n.setNum((polygonColor.rgb()<<8)|polygonColor.alpha(),16);
    while(n.length()<8){
        n.prepend('0');
    }
    n.prepend('#');
    o.insert("color",n);
}

void Polygon::removePoint(Triangle::index_type index){
    assert(index<points.size()&&"Index out of Bounds.");
    points.erase(points.begin()+index);
    for(auto i = triangles.begin();i!=triangles.cend();++i){
        if(i->haveIndex(index)){
            i = triangles.erase(i);
        }else{
            i->decrementAbove(index);
        }
    }
}
void Polygon::removePoint(PointContainer::const_iterator iter){
    assert(iter!=points.cend()&&"Index out of Bounds.");
    points.erase(iter);
    auto index = iter-points.cbegin();
    for(auto i = triangles.begin();i!=triangles.cend();++i){
        if(i->haveIndex(index)){
            i = triangles.erase(i);
        }else{
            i->decrementAbove(index);
        }
    }
}

void Polygon::addTriangle(Triangle::index_type first, Triangle::index_type second, Triangle::index_type third){
    triangles.emplace_back(first,second,third);
    update();
}
void Polygon::addArc(Arc::index_type first, Arc::index_type second, Arc::index_type third){
    arcs.emplace_back(first,second,third);
    update();
}

QSGNode* Polygon::updatePaintNode(QSGNode *oldNode , UpdatePaintNodeData *){
    QSGGeometryNode *node = nullptr;
    QSGGeometry *geometry = nullptr;


    static const auto fiveDegrees = 10/180.*M_PI;

    int extraVertexCountForArcs = 0;
    for(const auto a : arcs){
        extraVertexCountForArcs+=a.angle(points)/fiveDegrees;
    }
    const int extraIndexCountForArcs = 3*arcs.size()+3*extraVertexCountForArcs;

    if (!oldNode) {
        node = new QSGGeometryNode;
        geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), points.size() + extraVertexCountForArcs,triangles.size()*3 +extraIndexCountForArcs);
        geometry->setDrawingMode(QSGGeometry::DrawTriangles);
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);


        QSGFlatColorMaterial *material = new QSGFlatColorMaterial;
        material->setColor(polygonColor);
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);

    } else {
        node = static_cast<QSGGeometryNode *>(oldNode);
        geometry = node->geometry();
        geometry->allocate(points.size()+extraVertexCountForArcs,triangles.size()*3+extraIndexCountForArcs);
    }

    std::memcpy(geometry->vertexDataAsPoint2D(),points.data(),points.size()*sizeof(PointContainer::value_type));
    std::memcpy(geometry->indexDataAsUShort(),triangles.data(),triangles.size()*sizeof(TriangleContainer::value_type));
    // add arcs
    int lastVertexIndex = points.size();
    QSGGeometry::Point2D *vertices = geometry->vertexDataAsPoint2D() + points.size();
    unsigned short *indices= geometry->indexDataAsUShort() + triangles.size()*3;
    class Matrix{
    public:
        // rotiert den Vector um 5 Grad nach rechts
        static void rotate5Degrees(Point &p){
            static const auto cos = std::cos(fiveDegrees);
            static const auto sin = std::sin(fiveDegrees);
            const auto newX = cos * p.x - sin * p.y;
            p.y = sin * p.x + cos * p.y;
            p.x = newX;
        }
    };
    for(const auto arc : arcs){
        const auto angle = arc.angle(points);

        if(angle < fiveDegrees){
            *indices = arc.left;
            *(++indices) = arc.mid;
            *(++indices) = arc.right;
            ++indices;
        }else{
            Point startVector = arc.startVector(points);
            const Point endVector = arc.endVector(points);
            const auto startLength = std::sqrt(startVector.x*startVector.x+startVector.y*startVector.y);
            const auto endLength = std::sqrt(endVector.x*endVector.x+endVector.y*endVector.y);
            const auto lengthDiff = (endLength-startLength)/startLength;
//#warning Mögliche Division durch null
            const auto lengthXDiff = lengthDiff ;
            const auto lengthYDiff = lengthDiff ;
            const int needExtraVertices = angle / fiveDegrees;
            // add first Vertex and triangle
            Matrix::rotate5Degrees(startVector);
            vertices->x = points[arc.mid].x + startVector.x + startVector.x * lengthXDiff * (fiveDegrees/angle);
            vertices->y = points[arc.mid].y + startVector.y + startVector.y * lengthYDiff * (fiveDegrees/angle);
            *indices = arc.left;
            *(++indices) = arc.mid;
            *(++indices) = lastVertexIndex;
            for(int i = 1;i<needExtraVertices;++i){
                Matrix::rotate5Degrees(startVector);
                const auto lengthMult = lengthDiff * ((i+1)*fiveDegrees/angle);
                const auto lengthXMult = lengthXDiff * ((i+1)*fiveDegrees/angle);
                const auto lengthYMult = lengthYDiff * ((i+1)*fiveDegrees/angle);
                ++vertices;
                vertices->x = points[arc.mid].x + startVector.x + startVector.x*lengthXMult;
                vertices->y = points[arc.mid].y + startVector.y + startVector.y*lengthYMult;
                *++indices = lastVertexIndex;
                *++indices = arc.mid;
                ++lastVertexIndex;
                *++indices = lastVertexIndex;

            }
            // add last closing triangle
            *++indices = lastVertexIndex;
            *++indices = arc.mid;
            *++indices = arc.right;
            ++lastVertexIndex;
            ++indices;
            ++vertices;
            /*
            Point startVector = arc.startVector(points);
            auto startAngle = arc.startAngle(points) + M_PI_2;
            const Point endVector = arc.endVector(points);
            const auto startLength = std::sqrt(startVector.x*startVector.x+startVector.y*startVector.y);
            const auto endLength = std::sqrt(endVector.x*endVector.x+endVector.y*endVector.y);
            const auto lengthDiff = (endLength-startLength)/startLength;
            const auto lengthXDiff = lengthDiff;
            const auto lengthYDiff = 0;
            const int needExtraVertices = angle / fiveDegrees;
            // add first Vertex and triangle
            vertices->x = points[arc.mid].x + std::sin(startAngle+fiveDegrees) * startLength * (1+lengthDiff * (fiveDegrees/angle));
            vertices->y = points[arc.mid].y - std::cos(startAngle+fiveDegrees) * startLength * (1+lengthDiff * (fiveDegrees/angle));
            *indices = arc.left;
            *(++indices) = arc.mid;
            *(++indices) = lastVertexIndex;
            for(int i = 2;i<=needExtraVertices;++i){
                const auto lengthMult = 1 + lengthDiff * (i*fiveDegrees/angle);
                const auto lengthXMult = 1 + lengthXDiff * (i*fiveDegrees/angle);
                const auto lengthYMult = 1 + lengthYDiff * (i*fiveDegrees/angle);
                ++vertices;
                vertices->x = points[arc.mid].x + std::sin(startAngle+i*fiveDegrees) * startLength * lengthXMult;
                vertices->y = points[arc.mid].y - std::cos(startAngle+i*fiveDegrees) * startLength * lengthYMult;
                *++indices = lastVertexIndex;
                *++indices = arc.mid;
                ++lastVertexIndex;
                *++indices = lastVertexIndex;

            }
            // add last closing triangle
            *++indices = lastVertexIndex;
            *++indices = arc.mid;
            *++indices = arc.right;
            ++lastVertexIndex;
            ++indices;
            ++vertices;*/
        }
    }
    // count
    //qDebug() << "vertices alloc : "<<geometry->vertexCount()<<" or "<<points.size() + extraVertexCountForArcs<< "  used : "<<(vertices-geometry->vertexDataAsPoint2D())<<'\n';
    //qDebug() << "indices alloc : "<<geometry->indexCount()<<"or"<<triangles.size()*3 +extraIndexCountForArcs<< "  used : "<<(indices-geometry->indexDataAsUShort())<<'\n';
    node->markDirty(QSGNode::DirtyGeometry);

    return node;
}

Polygon::Triangle::index_type Polygon::createPoint(Point p){
    for(auto i = points.cbegin();i!=points.cend();++i){
        if(std::is_floating_point<Point::number_type>::value){
            if(i->isNear(p.x,p.y,std::numeric_limits<Point::number_type>::epsilon()*2)){
                return i-points.cbegin();
            }
        }else{
            if(i->x==p.x&&i->y==p.y){
                return i-points.cbegin();
            }
        }
    }
    points.push_back(p);
    return points.size()-1;
}

void Polygon::addRectangle(Point p1, Point p2, Point p3, Point p4){
    Triangle::index_type i1 = createPoint(p1),i2 = createPoint(p2),i3 = createPoint(p3),i4 = createPoint(p4);
    triangles.emplace_back(i1,i2,i3);
    triangles.emplace_back(i2,i3,i4);
}


}
