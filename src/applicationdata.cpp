#include "applicationdata.h"
#include "errornotifier.h"
#include "idbase.h"
#include "usermanagment.h"
#include "dmx/channel.h"
#include "dmx/device.h"
#include "dmx/deviceprototype.h"
#include "dmx/programm.h"
#include "dmx/programmprototype.h"
#include "gui/controlpanel.h"
#include "gui/mapview.h"
#include "modules/modulemanager.h"
#include "modules/programblock.h"
#include "spotify/spotify.h"
#include <QCryptographicHash>
#include <QJsonArray>
#include <QJsonDocument>

namespace ApplicationData {

bool saveData(const QString &filename) {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    auto data = saveData();
    file.write(data);
    file.close();
    return true;
}

template <typename Vector>
void saveIDBaseObjects(QJsonObject &o, const Vector &vector, const QString &entryName) {
    QJsonArray array;
    for (const auto &p : vector) {
        QJsonObject o;
        p->writeJsonObject(o);
        array.append(o);
    }
    o.insert(entryName, array);
}

QByteArray saveData() {
    qDebug() << "SAVE DATA";
    QJsonObject o;
    saveIDBaseObjects(o, ModelManager::get().getDevicePrototypes(), QStringLiteral("DevicePrototypes"));
    saveIDBaseObjects(o, ModelManager::get().getDevices(), QStringLiteral("Devices"));
    saveIDBaseObjects(o, ModelManager::get().getProgramPrototypes(), QStringLiteral("ProgrammPrototypes"));
    saveIDBaseObjects(o, ModelManager::get().getPrograms(), QStringLiteral("Programms"));
    saveIDBaseObjects(o, UserManagment::get()->getUsers(), QStringLiteral("Users"));
    if (GUI::ControlPanel::getLastCreated()) {
        QJsonObject u;
        GUI::ControlPanel::getLastCreated()->writeJsonObject(u);
        o.insert(QStringLiteral("ControlPanel"), u);
    }
    if (GUI::MapView::getLastCreated()) {
        QJsonObject u;
        GUI::MapView::getLastCreated()->writeJsonObject(u);
        o.insert(QStringLiteral("MapView"), u);
    }
    {
        QJsonObject u;
        Modules::ModuleManager::singletone()->writeJsonObject(u);
        o.insert(QStringLiteral("ModuleManager"), u);
    }
    {
        QJsonObject u;
        Modules::ProgramBlockManager::writeToJsonObject(u);
        o.insert(QStringLiteral("ProgramBlockManager"), u);
    }
    {
        QJsonObject u;
        Spotify::get().writeToJsonObject(u);
        o.insert(QStringLiteral("Spotify"), u);
    }
    return QJsonDocument(o).toJson();
}

std::pair<std::function<void()>, QString> loadData(const QString &filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return {{}, "Failed to open settings file " + file.fileName() + ". Error : " + file.errorString()};
    }
    auto r = loadData(file.readAll());
    file.close();
    return r;
}

std::pair<std::function<void()>, QString> loadData(const QByteArray &data) {
    QJsonParseError error{};
    auto doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError) {
        // compute line and column
        int line = 1;
        auto lastNewLine = data.cbegin();
        for (auto i = data.cbegin(); i != data.cbegin() + error.offset; ++i) {
            if (*i == '\n') {
                ++line;
                lastNewLine = i;
            }
        }
        int column = QString::fromUtf8(lastNewLine, data.cbegin() + error.offset - lastNewLine).length();
        return {{}, "Failed to parse settings file: " + error.errorString() + " at position " + QString::number(error.offset) + ", Ln " + QString::number(line) + ", Col " + QString::number(column)};
    }
    const auto o = doc.object();
    for (const auto e : o[QStringLiteral("DevicePrototypes")].toArray()) {
        ModelManager::get().addNewDevicePrototype(e.toObject());
    }
    for (const auto e : o[QStringLiteral("Devices")].toArray()) {
        ModelManager::get().addNewDevice(e.toObject());
    }
    for (const auto e : o[QStringLiteral("ProgrammPrototypes")].toArray()) {
        ModelManager::get().addNewProgramPrototype(e.toObject());
    }
    for (const auto e : o[QStringLiteral("Programms")].toArray()) {
        ModelManager::get().addNewProgram(e.toObject());
    }
    { // USERS
        for (const auto e : o[QStringLiteral("Users")].toArray()) {
            User::createUser(e.toObject());
        }
    }
    {
        QJsonObject ob = o[QStringLiteral("ModuleManager")].toObject();
        Modules::ModuleManager::singletone()->loadModules(ob);
    }
    Spotify::get().loadFromJsonObject(o[QStringLiteral("Spotify")].toObject());
    return {[=]() {
                if (GUI::MapView::getLastCreated()) {
                    GUI::MapView::getLastCreated()->loadFromJsonObject(o[QStringLiteral("MapView")].toObject());
                }
                Modules::ProgramBlockManager::readFromJsonObject(o[QStringLiteral("ProgramBlockManager")].toObject());
                if (GUI::ControlPanel::getLastCreated()) {
                    GUI::ControlPanel::getLastCreated()->loadFromJsonObject(o[QStringLiteral("ControlPanel")].toObject());
                }
            },
            {}};
}

} // namespace ApplicationData
