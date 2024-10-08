#include "driver.h"
#include "dmxchannelfilter.h"
#include "errornotifier.h"
#include "programm.h"
#include "span.h"
#include "dmx/artnet_server.h"
#include "modules/dmxconsumer.h"
#include <QDebug>
#include <QDir>
#include <QLibrary>
#include <QQmlEngine>
#include <cstring>

// #define LOG_DRIVER

#ifdef LOG_DRIVER
#include "device.h"
#include <QDateTime>
#include <algorithm>
#include <fstream>
#endif
#ifdef Q_OS_WIN
#include "susbdmxdriver.h"
#include <windows.h>
#endif

std::unique_ptr<DMX::HardwareInterface> driver;
ArtNetReceiver *artnet_receiver = nullptr;

#ifdef LOG_DRIVER
std::ofstream debugOutput;
std::vector<unsigned char> oldData;
#endif

namespace DMX {

namespace Driver {

    DMXQMLValue::DMXQMLValue() {
        QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    }

    DMXValueModel::DMXValueModel() {
        QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    }

    void DMXValueModel::setQMLEngineThread(QThread *qmlEngineThread) {
        if (this->qmlEngineThread == qmlEngineThread) return;
        this->qmlEngineThread = qmlEngineThread;
        for (auto &v : values) {
            v->moveToThread(qmlEngineThread);
        }
    }

    void DMXValueModel::setValues(unsigned char *v, size_t size) {
        if (!enableUpdates) return;
        int diffSize = static_cast<int>(size - values.size());
        if (diffSize > 0) {
            beginInsertRows(QModelIndex(), static_cast<int>(values.size()), static_cast<int>(values.size()) + diffSize);
            for (int i = 0; i < diffSize; ++i) {
                values.emplace_back(std::make_unique<DMXQMLValue>());
                if (qmlEngineThread) values.back()->moveToThread(qmlEngineThread);
            }
            emit valuesChanged();
        } else if (diffSize < 0) {
            beginRemoveRows(QModelIndex(), static_cast<int>(values.size()) + diffSize, static_cast<int>(values.size()) - 1);
            values.resize(size);
            emit valuesChanged();
        }
        if (diffSize > 0) {
            endInsertRows();
        } else if (diffSize < 0) {
            endRemoveRows();
        }
        for (auto dmxValue = values.begin(); dmxValue != values.end(); ++dmxValue, ++v) {
            **dmxValue = *v;
        }
    }

    QHash<int, QByteArray> DMXValueModel::roleNames() const {
        auto map = QAbstractListModel::roleNames();
        map.insert(ValueRole, "value");
        return map;
    }

    QVariant DMXValueModel::data(const QModelIndex &index, int role) const {
        Q_UNUSED(role)
        if (index.row() < 0 || index.row() >= rowCount()) return {};
        return QVariant::fromValue(values[static_cast<std::vector<unsigned char>::size_type>(index.row())].get());
    }

    DMXQMLValue *DMXValueModel::value(int index) {
        if (static_cast<size_t>(index) >= values.size()) return nullptr;
        return values[static_cast<size_t>(index)].get();
    }

    void initHardwareInterfaceCallbacks(DMX::HardwareInterface &inter) {
        inter.setErrorCallback([](QString s) {
            qDebug() << s;
            ErrorNotifier::get()->newError(s);
        });
#ifdef LOG_DRIVER
        debugOutput.open("driverOutput" + QDateTime::currentDateTime().toString("dd.MM.yyyy HH.mm.ss").toStdString() + ".csv");
        if (!debugOutput.is_open()) {
            qDebug() << "Can not open "
                     << "C:\\Program Files\\Lichtsteuerung\\data\\driverOutput" << QDateTime::currentDateTime().toString("dd.MM.yyyy HH.mm.ss") << ".csv";
        } else {
            std::vector<std::string> names(512, "Not set");
            for (int i = 0; i < names.size(); ++i) {
                names[i] += std::to_string(i + 1);
            }
            for (const auto &i : IDBase<Device>::getAllIDBases()) {
                if (i->getStartDMXChannel() > 1) names[i->getStartDMXChannel() - 1] = i->getName().toStdString();
            }
            debugOutput << ';';
            std::for_each(std::begin(names), std::end(names), [&](const auto &v) { debugOutput << v << ';'; });
            debugOutput << '\n';
        }
#endif
        inter.setSetValuesCallback([](unsigned char *values, int size, double time) {
#ifdef LOG_DRIVER
            oldData.resize(size);
#endif
            std::memset(values, 0, size);
            if (artnet_receiver) {
                artnet_receiver->writeDefaultValuesTo(span<unsigned char>(values, size));
            }
            DMXChannelFilter::initValues(values, size);
            Programm::fill(values, size, time);
            Modules::DMXConsumer::fillWithDMXConsumer(values, size);
            DMXChannelFilter::filterValues(values, size);
            if (artnet_receiver) {
                artnet_receiver->writeKeepValues(span<unsigned char>(values, size));
                artnet_receiver->writeOverrideValues(span<unsigned char>(values, size));
            }
            Driver::dmxValueModel.setValues(values, static_cast<size_t>(size));
#ifdef LOG_DRIVER
            if (debugOutput.is_open() && !std::equal(std::begin(oldData), std::end(oldData), values)) {
                debugOutput << std::to_string(time) << ';';
                std::for_each(values, values + size, [&](const auto &v) { debugOutput << static_cast<int>(v) << ';'; });
                debugOutput << '\n';
                std::copy(values, values + size, std::begin(oldData));
            }
#endif
        });
    }

    bool startDriver(std::unique_ptr<HardwareInterface> newdriver) {
        if (newdriver) {
            initHardwareInterfaceCallbacks(*newdriver);
            if (!newdriver->init()) return false;
            stopAndUnloadDriver();
            newdriver->start();
            driver = std::move(newdriver);
            return true;
        }
        return false;
    }

    bool loadAndStartDriver(QString path) {
        auto driver = loadDriver(path);
        if (!driver) {
            return false;
        }
        return startDriver(std::move(driver));
    }

    std::unique_ptr<DMX::HardwareInterface> loadDriver(QString path) {
#if defined(Q_OS_WIN)
        typedef HardwareInterface *(*getDriverFunc)();
        // we have to load the library with the Windows API to handle dependecies in the same folder, see https://bugreports.qt.io/projects/QTBUG/issues/QTBUG-74001
        path.replace("/", "\\");
        auto pathC = path.toStdString();
        auto handle = LoadLibraryExA(pathC.c_str(), nullptr, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
        if (handle == nullptr) {
            auto errorCode = GetLastError();
            qDebug() << "Loading of lib failed, error code: " << errorCode;
            if (errorCode == 126)
                ErrorNotifier::get()->newError("Fehler beim laden der DLL, Abhängigkeiten zu anderen DLLs konnten nicht geladen werden.");
            else {
                ErrorNotifier::get()->newError("Fehler beim laden der DLL. Error code: " + QString::number(errorCode));
            }
            return nullptr;
        }
        auto func = GetProcAddress(handle, "getDriver");
        if (func == nullptr) {
            auto errorCode = GetLastError();
            qDebug() << "Loading of lib failed, error code: " << errorCode;
            ErrorNotifier::get()->newError("Fehler beim laden der Funktion \"getDriver\" aus der DLL, Fehlercode: " + QString::number(errorCode));
            return nullptr;
        }
        getDriverFunc getDriver = reinterpret_cast<getDriverFunc>(func);
        // getDriverFunc getDriver =  reinterpret_cast<getDriverFunc>(QLibrary::resolve(path,"getDriver"));
        if (getDriver != nullptr) {
            HardwareInterface *inter = getDriver();
            if (inter != nullptr) {
                stopAndUnloadDriver();
                return std::unique_ptr<DMX::HardwareInterface>(inter);
            }
        } else {
            if (!QFile::exists(path)) {
                ErrorNotifier::get()->newError("Driver File not existing : " + path + '\n' + "Home dir is : " + QDir::currentPath());
            } else {
                ErrorNotifier::get()->newError("Can not resolve Function getDriver in Library at Location " + path);
            }
        }

#else
        Q_UNUSED(path)
#warning Driverloading is only supported for Windows
#endif
        return nullptr;
    }

    HardwareInterface *getCurrentDriver() {
        return driver.get();
    }

    void stopAndUnloadDriver() {
        if (driver) {
            driver->stop();
            driver = nullptr;
        }
    }

    void useDataFromArtNetReceiver(ArtNetReceiver *receiver) {
        artnet_receiver = receiver;
    }

#ifdef WIN32
    bool startSUsbDMXDriver() {
        stopAndUnloadDriver();
        return startDriver(std::make_unique<SUsbDMXDriver>());
    }
#endif

} // namespace Driver

} // namespace DMX
