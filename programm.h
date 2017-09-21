#ifndef PROGRAMM_H
#define PROGRAMM_H

#include "programmprototype.h"
#include "device.h"

class Programm : public NamedObject, public IDBase<Programm>
{
    Q_OBJECT
    Q_PROPERTY(bool running READ isRunning WRITE setRunning NOTIFY runningChanged RESET stop)
    bool isRunning_ = false;
    class DeviceProgramm{
    private:
        ProgrammPrototype * programmPrototype;
    public:
        const Device * device;
        double offset;
        DeviceProgramm(Device * device,ProgrammPrototype * programmPrototype,double offset):programmPrototype(programmPrototype),device(device),offset(offset){}
        bool setProgrammPrototype(ProgrammPrototype * p){
            if(p->devicePrototype!=device->prototype){
                return false;
            }
            programmPrototype = p;
            return true;
        }
        ProgrammPrototype * getProgrammPrototyp()const{return programmPrototype;}
    };
    std::vector<DeviceProgramm> programms;
public:
    Programm(const QJsonObject &o);
    Programm(QString name,QString description = ""):NamedObject(name,description){}
    Q_SLOT void setRunning(bool run){if(run != isRunning_)emit runningChanged(run);isRunning_ = run;}
    void run(){if(!isRunning_)emit runningChanged(true);isRunning_ = true;}
    void stop(){if(isRunning_)emit runningChanged(false);isRunning_ = false;}
    bool isRunning(){return isRunning_;}
    static void fill(unsigned char * data, size_t length, double time);
    void writeJsonObject(QJsonObject &o)const;
    const std::vector<DeviceProgramm>& getDeviceProgramms()const{return programms;}
    bool addDeviceProgramm(Device * device, ProgrammPrototype * programmPrototype, double offset = 0);
signals:
    void runningChanged(bool running);
};

#endif // PROGRAMM_H
