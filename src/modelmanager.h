#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include "applicationdata.h"
#include "modelvector.h"
#include "settings.h"
#include "usermanagment.h"
#include "dmx/device.h"
#include "dmx/programm.h"
#include "dmx/programmprototype.h"
#include "modules/modulemanager.h"
#include <QObject>
#include <QQmlEngine>

class ModelManager : public QObject {
    Q_OBJECT
    Settings *settings = nullptr;
    ModelVector<std::unique_ptr<DMX::Channel>> channel;
    ModelVector<std::unique_ptr<DMX::Device>> devices;
    ModelVector<std::unique_ptr<DMX::DevicePrototype>> devicePrototypes;
    ModelVector<std::unique_ptr<DMX::Programm>> programs;
    ModelVector<std::unique_ptr<DMX::ProgrammPrototype>> programPrototypes;

    Q_PROPERTY(QAbstractItemModel *devices READ getDeviceModel CONSTANT)
    Q_PROPERTY(QAbstractItemModel *devicePrototypes READ getDevicePrototypeModel CONSTANT)
    Q_PROPERTY(QAbstractItemModel *programs READ getProgramModel CONSTANT)
    Q_PROPERTY(QAbstractItemModel *programPrototypes READ getProgramPrototypeModel CONSTANT)
private:
    ModelManager() = default;

public:
    static ModelManager &get() {
        static ModelManager singleton;
        return singleton;
    }
    const ModelVector<std::unique_ptr<DMX::Device>> &getDevices() { return devices; }
    const ModelVector<std::unique_ptr<DMX::DevicePrototype>> &getDevicePrototypes() { return devicePrototypes; }
    const ModelVector<std::unique_ptr<DMX::Programm>> &getPrograms() { return programs; }
    const ModelVector<std::unique_ptr<DMX::ProgrammPrototype>> &getProgramPrototypes() { return programPrototypes; }
    QAbstractItemModel *getDeviceModel() { return &devices; }
    QAbstractItemModel *getDevicePrototypeModel() { return &devicePrototypes; }
    QAbstractItemModel *getProgramModel() { return &programs; }
    QAbstractItemModel *getProgramPrototypeModel() { return &programPrototypes; }
    DMX::Device *getDeviceById(const QJsonValue &id) { return getDeviceById(id.toString().toLongLong()); }
    DMX::Device *getDeviceById(ID id) { return getDeviceById(id.value()); }
    DMX::Device *getDeviceById(ID::value_type id);
    DMX::DevicePrototype *getDevicePrototypeById(const QJsonValue &id) { return getDevicePrototypeById(id.toString().toLongLong()); }
    DMX::DevicePrototype *getDevicePrototypeById(ID id) { return getDevicePrototypeById(id.value()); }
    DMX::DevicePrototype *getDevicePrototypeById(ID::value_type id);
    DMX::Programm *getProgramById(const QJsonValue &id) { return getProgramById(id.toString().toLongLong()); }
    DMX::Programm *getProgramById(ID id) { return getProgramById(id.value()); }
    DMX::Programm *getProgramById(ID::value_type id);
    DMX::ProgrammPrototype *getProgramPrototypeById(const QJsonValue &id) { return getProgramPrototypeById(id.toString().toLongLong()); }
    DMX::ProgrammPrototype *getProgramPrototypeById(ID id) { return getProgramPrototypeById(id.value()); }
    DMX::ProgrammPrototype *getProgramPrototypeById(ID::value_type id);
    template <typename... Args>
    void addNewDevice(Args &&...args) {
        devices.push_back(std::make_unique<DMX::Device>(std::forward<Args>(args)...));
        QQmlEngine::setObjectOwnership(devices.back().get(), QQmlEngine::CppOwnership);
    }
    template <typename... Args>
    void addNewDevicePrototype(Args &&...args) {
        devicePrototypes.push_back(std::make_unique<DMX::DevicePrototype>(std::forward<Args>(args)...));
        QQmlEngine::setObjectOwnership(devicePrototypes.back().get(), QQmlEngine::CppOwnership);
    }
    template <typename... Args>
    void addNewProgramPrototype(Args &&...args) {
        programPrototypes.push_back(std::make_unique<DMX::ProgrammPrototype>(std::forward<Args>(args)...));
        QQmlEngine::setObjectOwnership(programPrototypes.back().get(), QQmlEngine::CppOwnership);
    }
    template <typename... Args>
    void addNewProgram(Args &&...args) {
        programs.push_back(std::make_unique<DMX::Programm>(std::forward<Args>(args)...));
        QQmlEngine::setObjectOwnership(programs.back().get(), QQmlEngine::CppOwnership);
    }
    Q_INVOKABLE void removeDmxDevice(DMX::Device *device) {
        if (device) devices.remove_if([=](const auto &p) { return p.get() == device; });
    }
    Q_INVOKABLE void removeDmxDevicePrototype(DMX::DevicePrototype *devicePrototype) {
        if (devicePrototype) devicePrototypes.remove_if([=](const auto &p) { return p.get() == devicePrototype; });
    }
    Q_INVOKABLE void removeDmxProgram(DMX::Programm *program) {
        if (program) programs.remove_if([=](const auto &p) { return p.get() == program; });
    }
    Q_INVOKABLE void removeDmxProgramPrototype(DMX::ProgrammPrototype *programPrototype) {
        if (programPrototype) programPrototypes.remove_if([=](const auto &p) { return p.get() == programPrototype; });
    }
    Q_INVOKABLE bool addDevice(int row, int startDMXChannel, QString name, QString desciption = "", QPoint position = QPoint(-1, -1)) {
        qDebug() << "addDevice : " << row << ' ' << startDMXChannel << ' ' << name << ' ' << desciption << '\n';
        if (static_cast<size_t>(row) <= devicePrototypes.size()) {
            addNewDevice(devicePrototypes[row].get(), startDMXChannel, name, desciption, position);
            return true;
        }
        return false;
    }
    Q_INVOKABLE bool addDevicePrototype(QString name, QString description = "") {
        addNewDevicePrototype(std::move(name), std::move(description));
        return true;
    }
    Q_INVOKABLE bool addProgrammPrototype(int row /*DevicePrototype*/, QString name, QString description = "") {
        if (static_cast<size_t>(row) <= devicePrototypes.size()) {
            addNewProgramPrototype(devicePrototypes[row].get(), name, description);
            return true;
        }
        return false;
    }
    Q_INVOKABLE bool addProgramm(QString name, QString description = "") {
        addNewProgram(name, description);
        return true;
    }
    Q_INVOKABLE bool addModule() {
        Modules::ModuleManager::singletone()->getModules()->push_back(new Modules::Module());
        qDebug() << "count : " << Modules::ModuleManager::singletone()->getModules()->size();
        return true;
    }
    Q_INVOKABLE bool duplicateModule(int index) {
        const auto vec = Modules::ModuleManager::singletone()->getModules();
        if (index >= 0 && index < static_cast<int>(vec->size())) {
            QJsonObject o;
            (*vec)[index]->writeJsonObject(o);
            Modules::ModuleManager::singletone()->getModules()->push_back(new Modules::Module(o));
            return true;
        }
        return false;
    }
    Q_INVOKABLE void removeModule(int index) {
        const auto vec = Modules::ModuleManager::singletone()->getModules();
        if (index >= 0 && index < static_cast<int>(vec->size())) {
            delete vec->erase(index);
        }
    }
    Q_INVOKABLE bool addProgramBlock() {
        auto temp = std::make_shared<Modules::ProgramBlock>();
        Modules::ProgramBlockManager::model.push_back(temp);
        return true;
    }
    Q_INVOKABLE void removeProgramBlock(int index) {
        auto &vec = Modules::ProgramBlockManager::model;
        if (index >= 0 && index < static_cast<int>(vec.size())) {
            vec.erase(index);
        }
    }
    void setSettings(Settings *s) { settings = s; }
    /**
     * @brief saves the application data
     */
    Q_INVOKABLE void save() {
        if (settings != nullptr) {
            QString savePath(settings->getJsonSettingsFilePath());
            if (QFile::exists(savePath)) {
                QFile::copy(savePath, savePath + "_" + QDateTime::currentDateTime().toString(QStringLiteral("dd.MM.yyyy HH.mm.ss")));
            }
            ApplicationData::saveData(savePath);
        } else {
            qWarning() << "Settings in ModelManager not set! We can not save data!";
        }
    }
};

#endif // MODELMANAGER_H
