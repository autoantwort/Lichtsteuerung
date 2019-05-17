#ifndef MAPVIEW_H
#define MAPVIEW_H

#include "polygon.h"
#include "gridbackground.h"
#include <iostream>
#include "scanner.h"
#include "modules/modulemanager.h"

namespace GUI{

class vector{
public:
    float x,y,z;
    vector(float x, float y, float z):x(x),y(y),z(z){}
    vector():x(0),y(0),z(0){}
    vector& normalize(){
        auto sum = length();
        x /= sum;
        y /= sum;
        z /= sum;
        return *this;
    }
    float length(){
        return std::sqrt(std::pow(x,2.f) +std::pow(y,2.f)+ std::pow(z,2.f));
    }
    vector & operator-=(const vector & other){
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return  *this;
    }
    operator QString() const { return QString("vector(") + QString::number(x) + ',' + QString::number(y) + ',' + QString::number(z) +")"; }
};


inline std::ostream & operator<<(std::ostream & o, const vector& v){
    o << "vector(" << v.x << ',' << v.y << ',' << v.z <<")";
    return o;
}

static inline vector scannerPosition;

inline vector scannerRay(float angleInRadians){
    return vector(0,std::cos(angleInRadians),std::sin(angleInRadians));
}

inline vector getVectorFromScannerToMouse(float x, float y){
    vector vec(x,y,0);
    vec -= scannerPosition;
    vec.normalize();
    return vec;
}

inline float toDegrees(float radians){
    return ( radians * 180.f ) / static_cast<float>(M_PI);
}
inline float toRadians(float degree){
    return degree * static_cast<float>(M_PI) / 180.f;
}

inline vector reflectVector(vector incomingVec, float beta, float alpha){
    beta = toRadians(beta);
    alpha = toRadians(alpha);
    incomingVec.normalize();
    float x = 2.f * std::sin(beta) * std::sin(beta);
    float y = 2.f * std::sin(beta) * -std::sin(alpha) * std::cos(beta);
    float z = 2.f * std::sin(beta) * std::cos(alpha) * std::cos(beta);
    incomingVec -= vector(x,y,z);
    qDebug() << "normal : " << vector(std::sin(beta),-std::sin(alpha) * std::cos(beta),std::cos(alpha) * std::cos(beta));
    return incomingVec;
}

inline void computeAnglesDebug(vector position){
    auto vecToScanner = vector(1,0,0);
    auto vecFromScanner = position.normalize();
    qDebug() << "computeAngles : ";
    qDebug() << vecToScanner;
    qDebug() << vecFromScanner;
    qDebug() << (vecFromScanner.x-1)/-2.f;
    qDebug() << -std::sqrt((vecFromScanner.x-1)/-2.f);
    auto beta = std::asin(-std::sqrt((vecFromScanner.x-1)/-2.f));
    qDebug() << beta;
    qDebug() << toDegrees(beta);
    qDebug() << "v : " << vector(std::sin(beta),0,std::cos(beta));;

    auto b = std::cos(beta) * std::sin(beta);
    auto alpha1 = std::asin(vecFromScanner.y / (2.f * b));
    qDebug() << toDegrees(alpha1);
    //auto alpha2 = std::acos(vecFromScanner.z / (-2.f * b));
    //qDebug() << toDegrees(alpha2);
}

inline void computeAngles(vector position){
    auto vecToScanner = vector(1,0,0);
    auto vecFromScanner = position.normalize();
    auto beta = std::asin(-std::sqrt((vecFromScanner.x-1)/-2.f));
    auto b = std::cos(beta) * std::sin(beta);
    auto alpha = std::asin(vecFromScanner.y / (2.f * b));
}

inline void test(){
    Scanner scanner(QVector3D(20,200,200),90,30);
    auto angles = scanner.computeAngleForStepMotorsForPositionOnMap(200,350);
    qDebug() << "alpha : " << angles.first << " betha : "<<angles.second;
    qDebug() << scanner.getTransformationMatrix() * QVector3D(40,220,0);
    scannerPosition = {20,200,200};
    //computeAngles(vector(0,0,1));
    //qDebug() << reflectVector(vector(1,0,0),-10,0);
    computeAngles(reflectVector(vector(1,0,0),-10,-45));
    computeAngles(vector(1,1,1));
}



class MapView : public GridBackground
{
    Q_OBJECT
    Q_PROPERTY(QPointF controlPoint READ getControlPoint WRITE setControlPoint NOTIFY controlPointChanged)
    QPointF controlPoint;
    static MapView * lastCreated;
protected:
    std::vector<GUI::Polygon *> polygons;
    GUI::Polygon * stonework;
    GUI::Polygon * surfaces;
public:
    MapView();
    static MapView * getLastCreated(){return lastCreated;}
    void loadFromJsonObject(const QJsonObject &o);
    void writeJsonObject(QJsonObject &o)const;
    void setControlPoint(const QPointF & p){
        if(p != controlPoint){
            controlPoint = p;
            emit controlPointChanged();
            Modules::ModuleManager::singletone()->controller().updateControlPoint(controlPoint);
        }
    }
    QPointF getControlPoint()const{return controlPoint;}
protected:
    void mousePressEvent(QMouseEvent *event)override;
    void mouseMoveEvent(QMouseEvent *event)override;
signals:
    void controlPointChanged();
public slots:
};

}

#endif // MAPVIEW_H
