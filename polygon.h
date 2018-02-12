#ifndef POLYGON_H
#define POLYGON_H
#include <limits>
#include <vector>
#include <QQuickItem>
#include <cmath>
#include <QJsonObject>

namespace Primitives {


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
public:
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
public:
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

}

/**
 * @brief The Polygon class holds an Polygon. Its contains multiple Points that con be combined to Triangles and Arcs
 */
class Polygon : public QQuickItem
{
public:
    typedef Primitives::Point<float> Point;
    typedef Primitives::Triangle<unsigned short> Triangle;
    typedef Primitives::Arc<unsigned short> Arc;
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
    QString name;
public:
    Polygon(QString name);
    Polygon(const QJsonObject &o);
    void writeJsonObject(QJsonObject &o)const;
    /**
     * @brief setColor Sets the Color with that the Polygon will be rendered
     * @param c The new Color of the Polygon
     */
    void setColor(QColor c){polygonColor=c;update();}
    /**
     * @brief getColor return the current color of the Polygon
     * @return The current polygon color
     */
    QColor getColor()const{return polygonColor;}
    /**
     * @brief setName Sets the Name of the Polygon, so that the User nows, for what this polygon stands for, eg stonework
     * @param c The new name of the polygon
     */
    void setName(QString c){name=c;}
    QString getName()const{return name;}
    const PointContainer & getPoints()const{return points;}
    TriangleContainer & getTriangles(){return triangles;}
    const TriangleContainer & getTriangles()const{return triangles;}
    const ArcContainer & getArcs()const{return arcs;}
    /**
     * @brief addPoint Adds a Point to the Polygon
     * @param p The new Point
     */
    void addPoint(Point &&p){
        points.push_back(p);
    }
    /**
     * @brief addPoint Adds a Point to the Polygon
     * @param p The new Point
     */
    void addPoint(const Point &p){
        points.push_back(p);
    }
    /**
     * @brief removePoint Removes the point at the given index, all Points after this Point decreses there index by one
     * @param index The Index of the Point in the PointContainer to remove
     */
    void removePoint(Triangle::index_type index);
    /**
     * @brief removePoint Removes the Point at the given index, all Points after this Point decreses there index by one
     * @param index An iterator position to the Point to remove
     */
    void removePoint(PointContainer::const_iterator index);
    /**
     * @brief addTriangle Adds a triangle with 3 given Points
     * @param first The first edge point of the triangle
     * @param second The second edge point of the triangle
     * @param third The third edge point of the triangle
     */
    void addTriangle(Triangle::index_type first, Triangle::index_type second, Triangle::index_type third);
    /**
     * @brief addArc Adds a Arc with the 3 given points
     * @param left The Left Point where the Arc starts
     * @param mid The midpoint of the Arc
     * @param right The Right Point where the Arc ends
     */
    void addArc(Arc::index_type left, Arc::index_type mid, Arc::index_type right);
protected:
    /**
     * @brief createPoint Adds a point if the Point does not exists and return the new index, or returns the index of the existins Point
     * @return the index of the Point in the Container
     */
    Triangle::index_type createPoint(Point );
    /**
     * @brief addRectangle Adds a Rectangle with 4 edge points
     */
    void addRectangle(Point,Point,Point,Point);
    /**
     * @brief addRectangle Adds a Rectangle with 4 edge Points
     * @param x1 The x coordinate of the first point
     * @param y1 The y coordinate of the first point
     * @param x2 The x coordinate of the second point
     * @param y2 The y coordinate of the second point
     * @param x3 The x coordinate of the third point
     * @param y3 The y coordinate of the third point
     * @param x4 The x coordinate of the fourth point
     * @param y4 The y coordinate of the fourth point
     */
    void addRectangle(Point::number_type x1,Point::number_type y1,Point::number_type x2,Point::number_type y2,Point::number_type x3,Point::number_type y3,Point::number_type x4,Point::number_type y4){
        addRectangle(Point(x1,y1),Point(x2,y2),Point(x3,y3),Point(x4,y4));
    }
    /**
     * @brief addRectangle adds a Rectanle given by the Left/Right x-Axies and the Top/Bottom y-xies
     * @param leftX The Left x
     * @param rightX The Right x
     * @param topY The top y
     * @param bottomY the bottom y
     */
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
