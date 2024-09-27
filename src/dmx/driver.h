#ifndef DRIVER_H
#define DRIVER_H

#include "HardwareInterface.h"
#include <QAbstractListModel>

class ArtNetReceiver;

namespace DMX {

namespace Driver {

    class DMXQMLValue : public QObject {
        Q_OBJECT
        Q_PROPERTY(int value READ getValue NOTIFY valueChanged)
        unsigned char value;

    public:
        DMXQMLValue();
        void operator=(unsigned char v) { setValue(v); }
        void setValue(unsigned char v) {
            if (v != value) {
                value = v;
                emit valueChanged();
            }
        }
        int getValue() const { return value; }
    signals:
        void valueChanged();
    };
    /**
     * @brief The DMXValueModel class is a QAbstractListModel wrapper around the values send from the Driver
     */
    class DMXValueModel : public QAbstractListModel {
        Q_OBJECT
        Q_PROPERTY(bool enableUpdates MEMBER enableUpdates NOTIFY enableUpdatesChanged)
        Q_PROPERTY(DMXValueModel *values READ getValues NOTIFY valuesChanged)
        std::vector<std::unique_ptr<DMXQMLValue>> values;
        QThread *qmlEngineThread = nullptr;
        /**
         * @brief enableUpdate only if enableUpdates is true the values gets updated in setValues
         */
        bool enableUpdates = true;

    public:
        DMXValueModel();
        enum {
            ValueRole = Qt::UserRole + 1,
        };
        /**
         * @brief setValues sets the values of the dmx output so that the values are available in qml
         * @param v the values
         * @param size the length of the values array
         */
        void setValues(unsigned char *v, size_t size);
        void setQMLEngineThread(QThread *qmlEngineThread);
        virtual QHash<int, QByteArray> roleNames() const override;
        virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override {
            Q_UNUSED(parent)
            return static_cast<int>(values.size());
        }
        virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
        Q_INVOKABLE DMXQMLValue *value(int index);
        DMXValueModel *getValues() { return values.empty() ? nullptr : this; }
    signals:
        void enableUpdatesChanged();
        void valuesChanged();
    };

    /**
     * @brief dmxValueModel a model to access the outgoing dmx values in qml
     */
    inline DMXValueModel dmxValueModel;
    /**
     * @brief loadAndStartDriver load see: @see loadDriver() starts the loaded driver
     * @param path a path to the dll, where the Funktion getDriver can be called
     * @return true for success, false for failure
     */
    bool loadAndStartDriver(QString path);
#ifdef WIN32
    /**
     * @brief startSUsbDMXDriver Starts the build in SUSBDMX Driver on windows
     * @return true for success, false for failure
     */
    bool startSUsbDMXDriver();
#endif
    /**
     * @brief loadDriver Loads the driver from a path and stop and delete the old driver
     * @param path The path to the dll where the driver can be loaded
     * @return true for success, false for failure
     */
    std::unique_ptr<HardwareInterface> loadDriver(QString path);
    /**
     * @brief startDriver Starts the driver and sets the nessesary callbacks. Stops the old driver before the new one is started
     * @param driver The driver that should be started
     * @return true is successfull
     */
    bool startDriver(std::unique_ptr<HardwareInterface> driver);
    /**
     * @brief getCurrentDriver return the current loaded driver
     * @return the driver or a nullptr, if no driver is loaded
     */
    HardwareInterface *getCurrentDriver();
    /**
     * @brief stopAndUnloadDriver stop and remove the driver, call this, when you close the application
     */
    void stopAndUnloadDriver();

    void useDataFromArtNetReceiver(ArtNetReceiver *);
} // namespace Driver

} // namespace DMX

#endif // DRIVER_H
