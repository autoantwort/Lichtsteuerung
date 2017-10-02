#ifndef PROGRAMM_H
#define PROGRAMM_H

#include "programmprototype.h"
#include "device.h"
#include "idbase.h"
#include <map>
#include <cmath>

class DeviceProgramm : public QObject , public IDBase<DeviceProgramm>{
    Q_OBJECT
    Q_PROPERTY(ProgrammPrototype* programmPrototype READ getProgrammPrototyp WRITE setProgrammPrototype NOTIFY programmPrototypeChanged)
    Q_PROPERTY(double offset READ getOffset WRITE setOffset NOTIFY offsetChanged)
    Q_PROPERTY(double speed READ getSpeed WRITE setSpeed NOTIFY speedChanged)
    Q_PROPERTY(Device* device READ getDevice CONSTANT)
private:
    ProgrammPrototype * programmPrototype;
    double offset;
    double speed = 1.0;
public:
    const Device * device;
    Device * getDevice()const{return const_cast<Device *>(device);}
    Q_SLOT void setOffset(double o){if(o==offset)return;offset=o;emit offsetChanged();SyncService::addUpdateMessage("DeviceProgramm",getID(),"offset",QString::number(offset));}
    double getOffset()const{return offset;}
    Q_SLOT void setSpeed(double s){if(s==speed)return;speed=s;emit speedChanged(speed);}
    double getSpeed()const{return speed;}
    DeviceProgramm(Device * device,ProgrammPrototype * programmPrototype,double offset):QObject(device),programmPrototype(programmPrototype),offset(offset),device(device){connect(programmPrototype,&ProgrammPrototype::destroyed,this,&DeviceProgramm::programmPrototypeDeleted);}
    bool setProgrammPrototype(ProgrammPrototype * p){
        if(p->devicePrototype!=device->prototype){
            return false;
        }
        disconnect(programmPrototype,&ProgrammPrototype::destroyed,this,&DeviceProgramm::programmPrototypeDeleted);
        programmPrototype = p;
        connect(programmPrototype,&ProgrammPrototype::destroyed,this,&DeviceProgramm::programmPrototypeDeleted);
        emit programmPrototypeChanged();
        SyncService::addUpdateMessage("DeviceProgramm",getID(),"programmPrototype",QString::number(programmPrototype->getID().value()));
        return true;
    }
    ProgrammPrototype * getProgrammPrototyp()const{return programmPrototype;}
private slots:
    void programmPrototypeDeleted(QObject *){delete this;}
public:
    static void update (const ID &id, const QString &name,const QString &value){
        auto d = IDBase<DeviceProgramm>::getIDBaseObjectByID(id);
        if(d){
            if (name=="offset") {
                d->setOffset(value.toDouble());
            }else if(name=="programmPrototype"){
                auto proto = IDBase<ProgrammPrototype>::getIDBaseObjectByID(value.toLong());
                if(proto) d->setProgrammPrototype(proto);
            }
        }
    }
signals:
    void offsetChanged();
    void speedChanged(double speed);
    void programmPrototypeChanged();
};

class TimeDistortion : public QObject{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QEasingCurve distortionCurve READ getDistortionCurve WRITE setDistortionCurve NOTIFY distortionCurveChanged)
    Q_PROPERTY(double intervall READ getIntervall WRITE setIntervall NOTIFY intervallChanged)
private:
    bool enabled;
    QEasingCurve distortionCurve;
    double intervall;
public:
    Q_SLOT void setEnabled(bool e){if(e==enabled)return;enabled=e;emit enabledChanged(enabled);}
    bool isEnabled()const{return enabled;}
    Q_SLOT void setDistortionCurve(QEasingCurve d){if(d==distortionCurve)return;distortionCurve=d;emit distortionCurveChanged(distortionCurve);}
    QEasingCurve getDistortionCurve()const{return distortionCurve;}
    Q_SLOT void setIntervall(double i){if(i==intervall)return;intervall=i;emit intervallChanged(intervall);}
    bool getIntervall()const{return intervall;}
public:
    double distort(double time){
        double diff = std::fmodf(time,intervall);
        double offset = time-diff;
        diff = distortionCurve.valueForProgress(diff/intervall)*intervall;
        return offset+diff;
    }
signals:
    void enabledChanged(bool);
    void distortionCurveChanged(QEasingCurve);
    void intervallChanged(double);
};

class Programm : public NamedObject, public IDBase<Programm>
{
    Q_OBJECT
    Q_PROPERTY(bool running READ isRunning WRITE setRunning NOTIFY runningChanged RESET stop)
    Q_PROPERTY(double speed READ getSpeed WRITE setSpeed NOTIFY speedChanged)
    Q_PROPERTY(TimeDistortion * timeDistortion READ getTimeDistortion CONSTANT)
    bool isRunning_ = false;
    double speed=1.0;
    std::vector<DeviceProgramm*> programms;
    TimeDistortion timeDistortion;
public:
    static QString syncServiceClassName;
    Programm(const QJsonObject &o);
    Programm(QString name,QString description = ""):NamedObject(name,description,&syncServiceClassName){}
    Q_SLOT void setRunning(bool run){if(run != isRunning_)emit runningChanged(run);isRunning_ = run;SyncService::addUpdateMessage("Programm",getID(),"running",run?"true":"false");}
    // Für jedes Programm für jedes Device für jeden Channel
    std::map<Programm*,std::map<Device*,std::vector<Channel*>>> run();
    Q_SLOT void setSpeed(double s){if(s==speed)return;speed=s;emit speedChanged(speed);}
    double getSpeed()const{return speed;}
    void stop(){setRunning(false);}
    bool isRunning()const{return isRunning_;}
    static void fill(unsigned char * data, size_t length, double time);
    void writeJsonObject(QJsonObject &o)const;
    const std::vector<DeviceProgramm*>& getDeviceProgramms()const{return programms;}
    bool addDeviceProgramm(Device * device, ProgrammPrototype * programmPrototype, double offset = 0);
    TimeDistortion * getTimeDistortion(){return &timeDistortion;}
private:
    void addDeviceProgramm(const QJsonObject &o);
private slots:
    void deviceProgrammDeleted(QObject*);
public:
    static void update (const ID &id, const QString &name,const QString &value){
        auto d = IDBase<Programm>::getIDBaseObjectByID(id);
        if(d){
            auto s = name.toLatin1();
            d->setProperty(s.data(),QVariant(value));
        }
    }
    static void create (const QJsonObject &o){new Programm(o);}
    static void remove (const ID &id){
        auto d = IDBase<Programm>::getIDBaseObjectByID(id);
        if(d){
            delete d;
        }
    }
    static void createMember (const ID &id,const QString &name,const QJsonObject &o){
        auto d = IDBase<Programm>::getIDBaseObjectByID(id);
        if(d&&name=="programms")d->addDeviceProgramm(o);
    }
    static void removeMember (const ID &pid,const QString &name,const ID &id){
        auto d = IDBase<Programm>::getIDBaseObjectByID(pid);
        if(d&&name=="programms"){
            for(auto i = d->programms.cbegin();i!=d->programms.cend();++i){
                if((**i).getID()==id){
                    delete *i;
                    d->programms.erase(i);
                    return;
                }
            }
        }
    }
signals:
    void runningChanged(bool running);
    void speedChanged(double speed);
    void deviceProgrammAdded(DeviceProgramm*);
    void deviceProgrammRemoved(DeviceProgramm*);
};

#endif // PROGRAMM_H
