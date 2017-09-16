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

    };
    std::vector<DeviceProgramm> programms;
public:
    Programm();
    void run(){isRunning_ = true;}
    void stop(){isRunning_ = false;}
    bool isRunning(){return isRunning_;}
    static void fill(unsigned char * data, size_t length, double time);
};

#endif // PROGRAMM_H
