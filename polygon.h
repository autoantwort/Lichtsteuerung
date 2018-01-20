#ifndef POLYGON_H
#define POLYGON_H
#include <limits>
#include <vector>
#include <QQuickItem>
#include <cmath>
#include <QJsonObject>

template<typename type>
class Point{
    static_assert(std::is_integral<type>::value||std::is_floating_point<type>::value,"Type must be an number");
public:
    Point(type x, type y):x(x),y(y){}
    Point(const QJsonObject &o):x(o["x"].toDouble()),y(o["y"].toDouble()){}
    typedef type number_type;
    type x,y;
    bool isNear(type x, type y, type maxDisstance)const{
        return (this->x-x)*(this->x-x) + (this->y-y)*(this->y-y) < maxDisstance*maxDisstance;
    }
    void writeJsonObject(QJsonObject &o)const{
        o.insert("x",x);
        o.insert("y",y);
    }
};

template<typename Indextype>
class Triangle{
  Indextype first, second, third;
public:
  typedef Indextype index_type;
  Triangle(Indextype first,Indextype second,Indextype third):first(first),second(second),third(third){}
  Triangle(const QJsonObject &o):first(o["first"].toInt()),second(o["second"].toInt()),third(o["third"].toInt()){}
  inline bool haveIndex(Indextype i)const{
      return first==i||second==i||third==i;
  }
  void decrementAbove(Indextype i){
      if(first>i)--first;
      if(second>i)--second;
      if(third>i)--third;
  }
  void writeJsonObject(QJsonObject &o)const{
      o.insert("first",first);
      o.insert("second",second);
      o.insert("third",third);
  }
};

template<typename Indextype>
class Arc{
  Indextype left, mid, right;
public:
  friend class Polygon;
  typedef Indextype index_type;
  Arc(Indextype left,Indextype mid,Indextype right):left(left),mid(mid),right(right){}
  Arc(const QJsonObject &o):left(o["left"].toInt()),mid(o["mid"].toInt()),right(o["right"].toInt()){}
  inline bool haveIndex(Indextype i)const{
      return left==i||mid==i||right==i;
  }
  void decrementAbove(Indextype i){
      if(left>i)--left;
      if(mid>i)--mid;
      if(right>i)--right;
  }
  template<typename Container>
  double angle(const Container &c)const{
      const auto end = endAngle(c);
      const auto start = startAngle(c);
      if(start>end){
          return M_PI*2+end-start;
      }else{
          return end-start;
      }
  }
  template<typename Container>
  double startAngle(const Container &c)const{
      return std::atan2(c[left].y-c[mid].y, c[left].x-c[mid].x);
  }
  template<typename Container>
  double endAngle(const Container &c)const{
      return std::atan2(c[right].y-c[mid].y, c[right].x-c[mid].x);
  }
  template<typename Container>
  typename Container::value_type startVector(const Container &c)const{
      return {c[left].x-c[mid].x, c[left].y-c[mid].y};
  }
  template<typename Container>
  typename Container::value_type endVector(const Container &c)const{
      return {c[right].x-c[mid].x, c[right].y-c[mid].y};
  }
  void writeJsonObject(QJsonObject &o)const{
      o.insert("left",left);
      o.insert("mid",mid);
      o.insert("right",right);
  }
};


class Polygon : public QQuickItem
{
public:
    typedef Point<float> Point;
    typedef Triangle<unsigned short> Triangle;
    typedef Arc<unsigned short> Arc;
    typedef std::vector<Point> PointContainer;
    typedef std::vector<Triangle> TriangleContainer;
    typedef std::vector<Arc> ArcContainer;
    friend class MapEditor;
    friend class MapView;
protected:
    PointContainer points;
    TriangleContainer triangles;
    ArcContainer arcs;
    QColor polygonColor;
public:
    Polygon();
    Polygon(const QJsonObject &o);
     void writeJsonObject(QJsonObject &o)const;
    void setColor(QColor c){polygonColor=c;update();}
    QColor getColor()const{return polygonColor;}
    const PointContainer & getPoints()const{return points;}
    TriangleContainer & getTriangles(){return triangles;}
    const TriangleContainer & getTriangles()const{return triangles;}
    const ArcContainer & getArcs()const{return arcs;}
    void addPoint(Point &&p){
        points.push_back(p);
    }
    void addPoint(const Point &p){
        points.push_back(p);
    }
    void removePoint(Triangle::index_type index);
    void removePoint(PointContainer::const_iterator index);
    void addTriangle(Triangle::index_type first, Triangle::index_type second, Triangle::index_type third);
    void addArc(Arc::index_type left, Arc::index_type mid, Arc::index_type right);
protected:
    Triangle::index_type createPoint(Point );
    void addRectangle(Point,Point,Point,Point);
    void addRectangle(Point::number_type x1,Point::number_type y1,Point::number_type x2,Point::number_type y2,Point::number_type x3,Point::number_type y3,Point::number_type x4,Point::number_type y4){
        addRectangle(Point(x1,y1),Point(x2,y2),Point(x3,y3),Point(x4,y4));
    }
    void addRectangle(Point::number_type leftX,Point::number_type rightX,Point::number_type topY,Point::number_type bottomY){
        addRectangle(Point(leftX,topY),Point(rightX,topY),Point(leftX,bottomY),Point(rightX,bottomY));
    }
    void addArc(Point::number_type lx,Point::number_type ly,Point::number_type mx,Point::number_type my,Point::number_type rx,Point::number_type ry){
        addArc(createPoint(Point(lx,ly)),createPoint(Point(mx,my)),createPoint(Point(rx,ry)));
    }
public:
    QSGNode * updatePaintNode(QSGNode *, UpdatePaintNodeData *)override;
};

#endif // POLYGON_H
