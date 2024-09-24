#include "susbdmxdriver.h"

#include "susbdmx.h"
#include <QDebug>
#include <QStringLiteral>

SUsbDMXDriver::~SUsbDMXDriver() {
    if (handle) {
        susbdmx_close(handle);
    }
}

bool SUsbDMXDriver::init() {
    if (!susbdmx_open(0, &handle)) {
        error(QStringLiteral("Error, cant open dmx interface"));
        return false;
    }
    return isFunctioning();
}

void SUsbDMXDriver::setValuesDeviceDriver() {
    if (handle) {
        setValues(dmxData, MAX_CHANNEL, time);
        const int errorCode = susbdmx_dmxtx(handle, 0, MAX_CHANNEL, dmxData);
        if (errorCode != 0 && errorCode != MAX_CHANNEL + 1) {
            qDebug() << "ERROR: Cannot set values." << errorCode;
        }
    }
}
