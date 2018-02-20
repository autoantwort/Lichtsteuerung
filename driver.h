#ifndef DRIVER_H
#define DRIVER_H

#include "settings.h"
#include "HardwareInterface.h"

namespace Driver {
    /**
     * @brief loadAndStartDriver load see: @see loadDriver() starts the loaded driver
     * @param path a path to the dll, where the Funktion getDriver can be called
     * @return true for success, false for failure
     */
    bool loadAndStartDriver(QString path);
    /**
     * @brief loadDriver Loads the driver from a path and stop and delete the old driver
     * @param path The path to the dll where the driver can be loaded
     * @return true for success, false for failure
     */
    bool loadDriver(QString path);
    /**
     * @brief getCurrentDriver return the current loaded driver
     * @return the driver or a nullptr, if no driver is loaded
     */
    HardwareInterface * getCurrentDriver();
    /**
     * @brief stopAndUnloadDriver stop and remove the driver, call this, when you close the application
     */
    void stopAndUnloadDriver();
}

#endif // DRIVER_H
