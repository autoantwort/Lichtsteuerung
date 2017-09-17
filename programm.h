#ifndef PROGRAMM_H
#define PROGRAMM_H

#include "programmprototype.h"
#include "device.h"

class Programm : public IDBase<Programm>
{
    QString name;
    QString description;
    bool isRunning_ = false;
    class DeviceProgramm{
    public:
        double offset;
        Device * device;
        ProgrammPrototype * programmPrototype;
        DeviceProgramm(double offset,Device * device,ProgrammPrototype * programmPrototype):offset(offset),device(device),programmPrototype(programmPrototype){}
    };
    std::vector<DeviceProgramm> programms;
public:
    Programm(const QJsonObject &o);
    void run(){isRunning_ = true;}
    void stop(){isRunning_ = false;}
    bool isRunning(){return isRunning_;}
    static void fill(unsigned char * data, size_t length, double time);
    void writeJsonObject(QJsonObject &o)const;
};

#endif // PROGRAMM_H
