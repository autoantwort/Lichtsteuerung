#ifndef SUSBDMXDRIVER_H
#define SUSBDMXDRIVER_H

#include "HardwareInterface.h"

#include <windows.h>

constexpr int MAX_CHANNEL = 512;

class SUsbDMXDriver : public DMX::AbstractHardwareInterface {
    HANDLE handle = nullptr;
    unsigned char dmxData[MAX_CHANNEL]{};

protected:
    void setValuesDeviceDriver() override;

public:
    SUsbDMXDriver() = default;
    ~SUsbDMXDriver() override;
    bool init() override;
    bool isFunctioning() override { return handle != nullptr; }
};

#endif // SUSBDMXDRIVER_H
