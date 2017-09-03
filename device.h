#ifndef DEVICE_H
#define DEVICE_H

#include "deviceprototype.h"
#include "QPoint"
#include "idbase.h"


/**
 * @brief The Device class Ein "echtes" Gerät, zb der Laser oben rechts über der Bier Theke
 */
class Device : public IDBase<Device>
{
protected:
    /**
     * @brief prototype Ein Pointer auf den Typ/Prototype, von dem das Gerät ist. (Ist es eine Lamp, ein Laser, .. ?)
     */
    DevicePrototype * prototype;
    /**
     * @brief name Der Name des Geräts
     */
    QString name;
    /**
     * @brief desciption Eine beschreibung des Geräts
     */
    QString desciption;
    /**
     * @brief position Die Position des Geräts, es wird eine Karte geben, wo dann dieses Gerät mit diese Position eingezeichnet wird
     */
    QPoint position;
    /**
     * @brief startDMXChannel Der Start DMX Channel
     */
    int startDMXChannel;
public:
    Device();
};

#endif // DEVICE_H
