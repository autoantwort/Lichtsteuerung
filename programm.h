#ifndef PROGRAMM_H
#define PROGRAMM_H

#include "programmprototype.h"
#include "device.h"
#include "idbase.h"
#include <map>
#include <cmath>
#include "modelvector.h"

/**
 * @brief The DeviceProgramm class saves a Device in combination with a ProgrammPrototype and an offset
 */
class DeviceProgramm : public QObject , public IDBase<DeviceProgramm>{
    Q_OBJECT
    Q_PROPERTY(ProgrammPrototype* programmPrototype READ getProgrammPrototyp WRITE setProgrammPrototype NOTIFY programmPrototypeChanged)
    Q_PROPERTY(double offset READ getOffset WRITE setOffset NOTIFY offsetChanged)
    Q_PROPERTY(double speed READ getSpeed WRITE setSpeed NOTIFY speedChanged)
    Q_PROPERTY(Device* device READ getDevice CONSTANT)
private:
    ProgrammPrototype * programmPrototype;
    double offset = 0.0;
    double speed = 1.0;
public:
    const Device * device;
    Device * getDevice()const{return const_cast<Device *>(device);}
    /**
     * @brief setOffset setzt den offset des DeviceProgramms, um diese zeit läuft dieses Programm nach dem Programm
     * @param o der offset
     */
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

/**
 * @brief The TimeDistortion class distort the time for a Programm
 */
class TimeDistortion : public QObject{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QEasingCurve distortionCurve READ getDistortionCurve WRITE setDistortionCurve NOTIFY distortionCurveChanged)
    Q_PROPERTY(double intervall READ getIntervall WRITE setIntervall NOTIFY intervallChanged)
private:
    bool enabled=false;
    QEasingCurve distortionCurve;
    double intervall=0;
public:
    TimeDistortion()=default;
    TimeDistortion(const QJsonObject &o);
    Q_SLOT void setEnabled(bool e){if(e==enabled)return;enabled=e;emit enabledChanged(enabled);}
    bool isEnabled()const{return enabled;}
    Q_SLOT void setDistortionCurve(QEasingCurve d){if(d==distortionCurve)return;distortionCurve=d;emit distortionCurveChanged(distortionCurve);}
    void writeJsonObject(QJsonObject &o)const;
    QEasingCurve getDistortionCurve()const{return distortionCurve;}
    /**
     * @brief setIntervall setzt das Intervall in dem die distortion Curve arbeitet, zu dem Zeitpunkten intervall * int ist die Zeit wieder "normal"
     * @param i das intervall
     */
    Q_SLOT void setIntervall(double i);
    double getIntervall()const{return intervall;}
public:
    double distort(double time){
        double diff = std::fmod(time,intervall); // wie weit sind wir in aktuellen intervall
        double offset = time-diff; // offset zum intervall start
        diff = distortionCurve.valueForProgress(diff/intervall)*intervall;// die verzerrung für den Aktuellen fortchritt im intervall mal die intervall größe
        return offset+diff; // den offset plus der verzerrte Fortschritt im intervall
    }
signals:
    void enabledChanged(bool);
    void distortionCurveChanged(QEasingCurve);
    void intervallChanged(double);
};

class DeviceProgrammVector : public ModelVector<DeviceProgramm>{
    Q_OBJECT
};

/**
 * @brief Ein Programm ist das am meinsten bündelnde object. Ein Programm besteht aus verschiedenen Device Programmen, die jeweils aus einem Device und einen ProgrammPrototype für eine bestimmte DevicePrototype Gruppe.
 * Ein Programm besitzt eine TimeDistortion einheit, die bestimmte bereiche des Programms schneller oder langsamer machen kann, außerdem besitzt ein Programm eine ganz normale geschwindigkeit, genau so wie die Darunter liegenden DeviveProgramme.
 */
class Programm : public NamedObject, public IDBase<Programm>
{
    Q_OBJECT
    Q_PROPERTY(bool running READ isRunning WRITE setRunning NOTIFY runningChanged RESET stop)
    Q_PROPERTY(double speed READ getSpeed WRITE setSpeed NOTIFY speedChanged)
    Q_PROPERTY(TimeDistortion * timeDistortion READ getTimeDistortion CONSTANT)
    Q_PROPERTY(DeviceProgrammVector * programs READ getProgramms CONSTANT)
    bool isRunning_ = false;
    double speed=1.0;
    DeviceProgrammVector programms;
    TimeDistortion timeDistortion;
public:
    static QString syncServiceClassName;
    Programm(const QJsonObject &o);
    Programm(QString name,QString description = ""):NamedObject(name,description,&syncServiceClassName){}
    Q_SLOT void setRunning(bool run){if(run != isRunning_)emit runningChanged(run);isRunning_ = run;SyncService::addUpdateMessage("Programm",getID(),"running",run?"true":"false");}

    /**
     * @brief run lässt das Programm laufen, wenn es keine Konflikte mit anderen laufenden Programmen gibt
     * @return Ein Container mit allen auftretenden Konflikten
     */
    std::map<Programm*,std::map<Device*,std::vector<Channel*>>> run();
    Q_SLOT void setSpeed(double s){if(s==speed)return;speed=s;emit speedChanged(speed);}
    double getSpeed()const{return speed;}
    void stop(){setRunning(false);}
    bool isRunning()const{return isRunning_;}
    /**
     * @brief fill füllt data mit den von den Laufenden Programmen laufenden values auf
     * @param data
     * @param length
     * @param time die aktualle zeit, an der sich die Programme gerade befinden sollen
     */
    static void fill(unsigned char * data, size_t length, double time);
    void writeJsonObject(QJsonObject &o)const;
    const std::vector<DeviceProgramm*>& getDeviceProgramms()const{return programms.getVector();}
    DeviceProgrammVector*getProgramms(){return &programms;}
    /**
     * @brief addDeviceProgramm fügt ein Device Programm dem Programm an
     * @param device Das Gerät
     * @param programmPrototype Der Programm Prototype, muss den gleichen DevicePrototypen wie das Device haben
     * @param offset Die Verzögerung des Programms start zur normalen zeit
     * @return true wenn erfolgreich, sonst false
     */
    Q_INVOKABLE bool addDeviceProgramm(Device * device, ProgrammPrototype * programmPrototype, double offset = 0);
    Q_INVOKABLE void removeDeviceProgramm(int index);
    TimeDistortion * getTimeDistortion(){return &timeDistortion;}
private:
    void addDeviceProgramm(const QJsonObject &o);
private slots:
    void deviceProgrammDeleted(QObject*);
public:
    // statische methoden für den syncService:
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
