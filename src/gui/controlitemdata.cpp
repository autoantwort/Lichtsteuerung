#include "controlitemdata.h"
#include <QJsonArray>
#include <QQmlEngine>
#include <QQuickItem>
#include <QTimer>
namespace GUI {

///////////////////////////////////////////////////
//////////   UserVisibilityModel   ////////////////
///////////////////////////////////////////////////

UserVisibilityModel::UserVisibilityModel(const QJsonObject &o) {
    const auto array = o["excludedUserIDs"].toArray();
    for (const auto o : array) {
        const auto obj = o.toObject();
        excudedUsers.emplace(obj);
    }
}

void UserVisibilityModel::writeJsonObject(QJsonObject &o) const {
    QJsonArray a;
    for (const auto &id : excudedUsers) {
        QJsonObject o;
        id.writeJsonObject(o);
        a.push_back(o);
    }
    o["excludedUserIDs"] = a;
}

bool UserVisibilityModel::isVisibleForCurrentUser() const {
    if (UserManagment::get()->getCurrentUser()->havePermission(UserManagment::Admin)) {
        return true;
    }
    return excudedUsers.find(UserManagment::get()->getCurrentUser()->getID()) == excudedUsers.end();
}

QVariant UserVisibilityModel::data(const QModelIndex &index, int role) const {
    if (index.row() < 0 && index.row() >= rowCount(index)) {
        return QVariant("Index Out of Range");
    }
    switch (role) {
    case VisibilityRole:
    case Qt::EditRole: return QVariant(excudedUsers.find(UserManagment::get()->getUsers()[index.row()]->getID()) == excudedUsers.end());
    case Qt::DisplayRole:
    case UserNameRole: return QVariant(UserManagment::get()->getUsers()[index.row()]->getUsername());
    default: return QVariant("Unknown role!");
    }
}

bool UserVisibilityModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (role == VisibilityRole) {
        if (index.row() >= 0 && index.row() < rowCount(index)) {
            const auto id = UserManagment::get()->getUsers()[index.row()]->getID();
            if (value.toBool()) {
                excudedUsers.erase(excudedUsers.find(id));
            } else {
                const bool isAdmin = UserManagment::get()->getUsers()[index.row()]->havePermission(UserManagment::Admin);
                if (!isAdmin) {
                    excudedUsers.insert(id);
                } else {
                    emit dataChanged(this->index(index.row()), this->index(index.row()), {VisibilityRole});
                }
            }
            return true;
        }
    }
    return false;
}

///////////////////////////////////////////////////
////////////   ControlItemData   //////////////////
///////////////////////////////////////////////////

ControlItemData::ControlItemData() {
    qWarning() << "ControlItemData created from default constructor";
}

ControlItemData::ControlItemData(Type t, QObject *parent) : QObject(parent), type(t) {
    QObject::connect(UserManagment::get(), &UserManagment::currentUserChanged, [this]() { emit isVisibleForUserChanged(); });
    registerRemoteProperties();
}

ControlItemData::ControlItemData(const QJsonObject &o, QObject *parent)
    : QObject(parent)
    , startXBlock(o["startXBlock"].toInt())
    , startYBlock(o["startYBlock"].toInt())
    , mobileStartXBlock(o["mobileStartXBlock"].toInt(startXBlock))
    , mobileStartYBlock(o["mobileStartYBlock"].toInt(startYBlock))
    , userVisibilityModel(o)
    , type(static_cast<Type>(o["type"].toInt())) {
    userChangedConnection = QObject::connect(UserManagment::get(), &UserManagment::currentUserChanged, [this]() {
        emit isVisibleForUserChanged();
        propertyChangedForRemote("isVisibleForUser", isVisibleForUser() ? "true" : "false");
    });
    registerRemoteProperties();
}

void ControlItemData::registerRemoteProperties() {
    ControlItemSync::get().registerControlItenData(this);
    const auto callback = [this](int &r, void (ControlItemData::*signal)() = nullptr) {
        return [this, &r, signal](const QString &s) {
            if (!UserManagment::get()->getCurrentUser()->havePermission(UserManagment::MOVE_CONTROL_ITEM)) return;
            bool ok;
            auto v = s.toInt(&ok);
            if (ok) {
                r = v;
                if (signal) {
                    emit(this->*signal)();
                }
            }
        };
    };
    setRemotePropertyChangeCallback("x", callback(startXBlock, &ControlItemData::startXBlockChanged));
    setRemotePropertyChangeCallback("y", callback(startYBlock, &ControlItemData::startYBlockChanged));
    setRemotePropertyChangeCallback("mobileX", callback(mobileStartXBlock));
    setRemotePropertyChangeCallback("mobileY", callback(mobileStartYBlock));
}

void ControlItemData::setRemotePropertyChangeCallback(const QString &name, std::function<void(const QString &)> callback) {
    ControlItemSync::get().setRemotePropertyChangeCallback(this, name, callback);
}

void ControlItemData::propertyChangedForRemote(const QString &name, const QString &value) {
    ControlItemSync::get().propertyChangedForRemote(this, name, value);
}

void ControlItemData::setInitialValuesForRemote(std::function<void(QString, QString)> setPropertyValue) {
    setPropertyValue("x", QString::number(startXBlock));
    setPropertyValue("y", QString::number(startYBlock));
    setPropertyValue("mobileX", QString::number(mobileStartXBlock));
    setPropertyValue("mobileY", QString::number(mobileStartYBlock));
    setPropertyValue("type", typeToString(type));
    setPropertyValue("isVisibleForUser", isVisibleForUser() ? "true" : "false");
}

void ControlItemData::writeJsonObject(QJsonObject &o) {
    o.insert("type", type);
    o.insert("startXBlock", startXBlock);
    o.insert("startYBlock", startYBlock);
    o.insert("mobileStartXBlock", mobileStartXBlock);
    o.insert("mobileStartYBlock", mobileStartYBlock);
    userVisibilityModel.writeJsonObject(o);
}

void ControlItemData::setStartXBlock(int i) {
    if (i != startXBlock) {
        startXBlock = i;
        emit startXBlockChanged();
        propertyChangedForRemote("x", QString::number(startXBlock));
    }
}
void ControlItemData::setStartYBlock(int i) {
    if (i != startYBlock) {
        startYBlock = i;
        emit startYBlockChanged();
        propertyChangedForRemote("y", QString::number(startYBlock));
    }
}

GUI::ControlItemData::~ControlItemData() {
    ControlItemSync::get().deregisterControlItenData(this);
    QObject::disconnect(userChangedConnection);
}

// start ProgrammControlItemData

void ProgrammControlItemData::registerRemoteProperties() {
    setRemotePropertyChangeCallback("speed", [this](const QString &s) {
        bool ok;
        double value = s.toDouble(&ok);
        if (!ok || value <= 0) {
            qWarning() << "Get invalid value " << s << " from remote as a program speed";
        } else if (programm) {
            programm->setSpeed(value);
        }
    });
    setRemotePropertyChangeCallback("running", [this](const QString &s) {
        if (programm) programm->setRunning(s == "true");
    });
    const auto connectProgram = [this]() {
        connections[0] = QObject::connect(programm, &DMX::Programm::nameChanged, [this]() { propertyChangedForRemote("name", programm->getName()); });
        connections[1] = QObject::connect(programm, &DMX::Programm::speedChanged, [this]() { propertyChangedForRemote("speed", QString::number(programm->getSpeed())); });
        connections[2] = QObject::connect(programm, &DMX::Programm::runningChanged, [this]() { propertyChangedForRemote("running", programm->isRunning() ? "true" : "false"); });
    };
    if (programm) connectProgram();

    QObject::connect(this, &ProgrammControlItemData::programmChanged, [this, connectProgram]() {
        for (auto c : connections)
            QObject::disconnect(c);
        if (programm) connectProgram();
    });
}

void ProgrammControlItemData::setInitialValuesForRemote(std::function<void(QString, QString)> setPropertyValue) {
    ControlItemData::setInitialValuesForRemote(setPropertyValue);
    setPropertyValue("name", programm ? programm->getName() : "null");
    setPropertyValue("speed", programm ? QString::number(programm->getSpeed()) : "null");
    setPropertyValue("running", programm ? programm->isRunning() ? "true" : "false" : "null");
}

ProgrammControlItemData::ProgrammControlItemData(DMX::Programm *p, QObject *parent) : ControlItemData(ControlItemData::PROGRAMM, parent), programm(p) {
    registerRemoteProperties();
}

ProgrammControlItemData::ProgrammControlItemData(const QJsonObject &o, QObject *parent) : ControlItemData(o, parent), programm(ModelManager::get().getProgramById(o["programm"])) {
    registerRemoteProperties();
}

void ProgrammControlItemData::writeJsonObject(QJsonObject &o) {
    ControlItemData::writeJsonObject(o);
    o.insert("programm", QString::number(programm->getID().value()));
}
void ProgrammControlItemData::setProgramm(DMX::Programm *p) {
    if (p != programm) {
        programm = p;
        emit programmChanged();
    }
}

// Group Model

GroupModel::GroupModel(QAbstractItemModel *deviceModel) : deviceModel(deviceModel) {
    assert(deviceModel != nullptr);
    QObject::connect(deviceModel, &QAbstractItemModel::rowsAboutToBeInserted, this, &GroupModel::handleRowsAboutToBeInserted);
    QObject::connect(deviceModel, &QAbstractItemModel::rowsAboutToBeRemoved, this, &GroupModel::handleRowsAboutToBeRemoved);
    QObject::connect(deviceModel, &QAbstractItemModel::rowsInserted, this, &GroupModel::handleRowsInserted);
    QObject::connect(deviceModel, &QAbstractItemModel::rowsRemoved, this, &GroupModel::handleRowsRemoved);
    enabled.resize(deviceModel->rowCount(QModelIndex()));
}

void GroupModel::handleRowsAboutToBeInserted(const QModelIndex &parent, int start, int end) {
    beginInsertRows(parent, start, end);
}
void GroupModel::handleRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end) {
    beginRemoveRows(parent, start, end);
}
void GroupModel::handleRowsInserted(const QModelIndex &parent, int first, int last) {
    Q_UNUSED(parent);
    enabled.insert(enabled.cbegin() + first, first - last, false);
    endInsertRows();
}
void GroupModel::handleRowsRemoved(const QModelIndex &parent, int first, int last) {
    Q_UNUSED(parent)
    enabled.erase(enabled.cbegin() + first, enabled.cend() + last);
    endRemoveRows();
}

// GroupControlItemData
void GroupControlItemData::setInitialValuesForRemote(std::function<void(QString, QString)> setPropertyValue) {
    ControlItemData::setInitialValuesForRemote(setPropertyValue);
    setPropertyValue("name", name);
}

GroupControlItemData::GroupControlItemData(const QJsonObject &o, QObject *parent) : ControlItemData(o, parent), name(o["name"].toString()) {
    const auto enabled = o["enabledDevices"].toArray();
    auto iter = ModelManager::get().getDevices().cbegin();
    int index = 0;
    for (const auto e_ : enabled) {
        const auto e = e_.toString().toLongLong();
        const auto start = iter;
        while (iter != ModelManager::get().getDevices().cend()) {
            if ((**iter).getID() == e) {
                groupModel.setEnabled(index, true);
                ++iter;
                ++index;
                continue;
            }
            ++iter;
            ++index;
        }
        iter = ModelManager::get().getDevices().cbegin();
        index = 0;
        while (iter != start) {
            if ((**iter).getID() == e) {
                groupModel.setEnabled(index, true);
                ++iter;
                ++index;
                continue;
            }
            ++iter;
            ++index;
        }
    }
}

void GroupControlItemData::writeJsonObject(QJsonObject &o) {
    QJsonArray a;
    auto prog = ModelManager::get().getDevices().cbegin();
    auto en = groupModel.getEnabledVector().cbegin();
    for (; en != groupModel.getEnabledVector().cend(); ++prog, ++en) {
        if (*en) {
            a.append(QString::number((**prog).getID().value()));
        }
    }
    o.insert("enabledDevices", a);
    o.insert("name", name);
    ControlItemData::writeJsonObject(o);
}

void GroupControlItemData::forEach(std::function<void(DMX::Device *)> f) {
    auto prog = ModelManager::get().getDevices().cbegin();
    auto en = groupModel.getEnabledVector().cbegin();
    for (; en != groupModel.getEnabledVector().cend(); ++prog, ++en) {
        if (*en) {
            f(prog->get());
        }
    }
}

void GroupControlItemData::setName(QString n) {
    if (name != n) {
        name = n;
        emit nameChanged();
        propertyChangedForRemote("name", name);
    }
}
// SwitchGroupControlItemData

void SwitchGroupControlItemData::initRemoteProperties() {
    setRemotePropertyChangeCallback("activated", [this](const QString &s) { setActivated(s == "true"); });
}

void SwitchGroupControlItemData::setInitialValuesForRemote(std::function<void(QString, QString)> setPropertyValue) {
    GroupControlItemData::setInitialValuesForRemote(setPropertyValue);
    setPropertyValue("activated", activated ? "true" : "false");
    setPropertyValue("activateCooldown", QString::number(activateCooldown));
    setPropertyValue("deactivateCooldown", QString::number(deactivateCooldown));
}

SwitchGroupControlItemData::SwitchGroupControlItemData(const QJsonObject &o, QObject *parent)
    : GroupControlItemData(o, parent)
    , activated(o["activated"].toBool())
    , activateCooldown(o["activateCooldown"].toInt())
    , deactivateCooldown(o["deactivateCooldown"].toInt()) {
    initRemoteProperties();
}

void SwitchGroupControlItemData::writeJsonObject(QJsonObject &o) {
    GroupControlItemData::writeJsonObject(o);
    o.insert("activated", activated);
    o.insert("activateCooldown", activateCooldown);
    o.insert("deactivateCooldown", deactivateCooldown);
}

void SwitchGroupControlItemData::setActivateCooldown(int c) {
    if (c != activateCooldown) {
        activateCooldown = c;
        emit activateCooldownChanged();
        propertyChangedForRemote("activateCooldown", QString::number(activateCooldown));
    }
}

void SwitchGroupControlItemData::setDeactivateCooldown(int c) {
    if (c != deactivateCooldown) {
        deactivateCooldown = c;
        emit deactivateCooldownChanged();
        propertyChangedForRemote("deactivateCooldown", QString::number(deactivateCooldown));
    }
}

void SwitchGroupControlItemData::setActivated(bool a) {
    if (a != activated) {
        activated = a;
        forEach([this](DMX::Device *d) {
            auto filter = d->getFilterForChannelindex(0);
            if (filter) filter->setValue(255 * activated);
        });
        emit activatedChanged();
        propertyChangedForRemote("activated", activated ? "true" : "false");
    }
}

// DimmerGroupCont,falserolItemData

void DimmerGroupControlItemData::initRemoteProperties() {
    setRemotePropertyChangeCallback("value", [this](const QString &s) {
        bool ok;
        auto v = s.toInt(&ok);
        if (ok && v >= 0 && v <= 255) {
            setValue(v, false);
        } else {
            propertyChangedForRemote("value", QString::number(value));
        }
    });

    QObject::connect(UserManagment::get(), &UserManagment::currentUserChanged,
                     [this]() { propertyChangedForRemote("canEditMinMaxMapping", UserManagment::get()->getCurrentUser()->havePermission(UserManagment::CHANGE_MIN_MAX_MAPPING) ? "true" : "false"); });

    const auto &mE = DMX::DMXChannelFilter::operationMetaEnum;
    setRemotePropertyChangeCallback("maxOperation", [this, mE](const QString &s) {
        if (UserManagment::get()->getCurrentUser()->havePermission(UserManagment::CHANGE_MIN_MAX_MAPPING)) {
            auto data = s.toUtf8();
            auto v = mE.keyToValue(data.data());
            if (v != -1) {
                setMaxOperation(static_cast<Operation>(v), false);
                return;
            }
        }
        propertyChangedForRemote("maxOperation", mE.valueToKey(maxOperation));
    });
    setRemotePropertyChangeCallback("minOperation", [this, mE](const QString &s) {
        if (UserManagment::get()->getCurrentUser()->havePermission(UserManagment::CHANGE_MIN_MAX_MAPPING)) {
            auto data = s.toUtf8();
            auto v = mE.keyToValue(data.data());
            if (v != -1) {
                setMinOperation(static_cast<Operation>(v), false);
                return;
            }
        }
        propertyChangedForRemote("minOperation", mE.valueToKey(minOperation));
    });

    setRemotePropertyChangeCallback("maxValue", [this](const QString &s) {
        bool ok;
        auto v = s.toInt(&ok);
        if (ok && v >= 0 && v <= 255) {
            setMaxValue(v, false);
        } else {
            propertyChangedForRemote("maxValue", QString::number(maxValue));
        }
    });
    setRemotePropertyChangeCallback("minValue", [this](const QString &s) {
        bool ok;
        auto v = s.toInt(&ok);
        if (ok && v >= 0 && v <= 255) {
            setMinValue(v, false);
        } else {
            propertyChangedForRemote("minValue", QString::number(minValue));
        }
    });

    setRemotePropertyChangeCallback("shouldOverride", [this](const QString &s) { shouldOverrideValue(s == "true", false); });
}

void DimmerGroupControlItemData::setInitialValuesForRemote(std::function<void(QString, QString)> setPropertyValue) {
    GroupControlItemData::setInitialValuesForRemote(setPropertyValue);
    setPropertyValue("value", QString::number(value));
    setPropertyValue("canEditMinMaxMapping", UserManagment::get()->getCurrentUser()->havePermission(UserManagment::CHANGE_MIN_MAX_MAPPING) ? "true" : "false");
    const auto &mE = DMX::DMXChannelFilter::operationMetaEnum;
    setPropertyValue("maxOperation", mE.valueToKey(maxOperation));
    setPropertyValue("minOperation", mE.valueToKey(minOperation));
    setPropertyValue("maxValue", QString::number(maxValue));
    setPropertyValue("minValue", QString::number(minValue));
    setPropertyValue("shouldOverride", shouldOverrideValue_ ? "false" : "true");
}

DimmerGroupControlItemData::DimmerGroupControlItemData(const QJsonObject &o)
    : GroupControlItemData(o)
    , maxOperation(static_cast<Operation>(o["maxOperation"].toInt()))
    , minOperation(static_cast<Operation>(o["minOperation"].toInt()))
    , maxValue(o["maxValue"].toInt())
    , minValue(o["minValue"].toInt())
    , value(o["value"].toInt())
    , shouldOverrideValue_(o["shouldOverride"].toBool()) {
    initRemoteProperties();
}

void DimmerGroupControlItemData::writeJsonObject(QJsonObject &o) {
    GroupControlItemData::writeJsonObject(o);
    o.insert("maxOperation", maxOperation);
    o.insert("minOperation", minOperation);
    o.insert("maxValue", maxValue);
    o.insert("minValue", minValue);
    o.insert("value", value);
    o.insert("shouldOverride", shouldOverrideValue_);
}

void DimmerGroupControlItemData::setMaxOperation(Operation o, bool updateRemote) {
    if (maxOperation != o) {
        maxOperation = o;
        forEach([=](DMX::Device *d) {
            auto filter = d->getFilterForChannelindex(0);
            if (filter) filter->setMaxOperation(o);
        });
        emit maxOperationChanged();
        if (updateRemote) propertyChangedForRemote("maxOperation", DMX::DMXChannelFilter::operationMetaEnum.valueToKey(maxOperation));
    }
}
void DimmerGroupControlItemData::setMinOperation(Operation o, bool updateRemote) {
    if (minOperation != o) {
        minOperation = o;
        forEach([=](DMX::Device *d) {
            auto filter = d->getFilterForChannelindex(0);
            if (filter) filter->setMinOperation(o);
        });
        emit minOperationChanged();
        if (updateRemote) propertyChangedForRemote("minOperation", DMX::DMXChannelFilter::operationMetaEnum.valueToKey(minOperation));
    }
}

void DimmerGroupControlItemData::setMaxValue(unsigned char o, bool updateRemote) {
    if (maxValue != o) {
        maxValue = o;
        forEach([=](DMX::Device *d) {
            auto filter = d->getFilterForChannelindex(0);
            if (filter) filter->setMaxValue(o);
        });
        emit maxValueChanged();
        if (updateRemote) propertyChangedForRemote("maxValue", QString::number(maxValue));
    }
}
void DimmerGroupControlItemData::setMinValue(unsigned char o, bool updateRemote) {
    if (minValue != o) {
        minValue = o;
        forEach([=](DMX::Device *d) {
            auto filter = d->getFilterForChannelindex(0);
            if (filter) filter->setMinValue(o);
        });
        emit minValueChanged();
        if (updateRemote) propertyChangedForRemote("minValue", QString::number(minValue));
    }
}

void DimmerGroupControlItemData::setValue(unsigned char o, bool updateRemote) {
    if (value != o) {
        value = o;
        forEach([=](DMX::Device *d) {
            auto filter = d->getFilterForChannelindex(0);
            if (filter) filter->setValue(o);
        });
        emit valueChanged();
        if (updateRemote) propertyChangedForRemote("value", QString::number(value));
    }
}
void DimmerGroupControlItemData::shouldOverrideValue(bool o, bool updateRemote) {
    if (shouldOverrideValue_ != o) {
        shouldOverrideValue_ = o;
        forEach([=](DMX::Device *d) {
            auto filter = d->getFilterForChannelindex(0);
            if (filter) filter->shouldOverrideValue(o);
        });
        emit shouldOverrideValueChanged();
        if (updateRemote) propertyChangedForRemote("shouldOverride", shouldOverrideValue_ ? "true" : "false");
    }
}

// start ProgramBlockControlItemData

void ProgramBlockControlItemData::initRemoteProperties() {
    setRemotePropertyChangeCallback("state", [this](const QString &s) {
        if (!program) return;
        if (s == "start")
            program->start();
        else if (s == "stop")
            program->stop();
        else if (s == "resume")
            program->resume();
        else if (s == "restart")
            program->restart();
        else if (s == "stop")
            program->stop();
        else if (s == "pause")
            program->pause();
        else
            qWarning() << "Get wrong value " << s << " for a remote program block state";

        Modules::ProgramBlock::statusMetaEnum.valueToKey(program->getStatus());
    });
    QObject::connect(program, &Modules::ProgramBlock::statusChanged, [this]() { propertyChangedForRemote("state", Modules::ProgramBlock::statusMetaEnum.valueToKey(program->getStatus())); });
    QObject::connect(program, &Modules::ProgramBlock::nameChanged, [this]() { propertyChangedForRemote("name", program->getName()); });
}

void ProgramBlockControlItemData::setInitialValuesForRemote(std::function<void(QString, QString)> setPropertyValue) {
    ControlItemData::setInitialValuesForRemote(setPropertyValue);
    setPropertyValue("state", Modules::ProgramBlock::statusMetaEnum.valueToKey(program->getStatus()));
    setPropertyValue("name", program->getName());
}

ProgramBlockControlItemData::ProgramBlockControlItemData(Modules::ProgramBlock *p, QObject *parent) : ControlItemData(ControlItemData::PROGRAM_BLOCK, parent) {
    setProgramBlock(p);
    initRemoteProperties();
}

ProgramBlockControlItemData::ProgramBlockControlItemData(const QJsonObject &o, QObject *parent) : ControlItemData(o, parent) {
    const auto id = o["programBlock"].toString().toLongLong();
    for (const auto &p : Modules::ProgramBlockManager::model) {
        if (p->getID() == id) {
            setProgramBlock(p.get());
            initRemoteProperties();
            return;
        }
    }
    throw std::runtime_error("No ProgramBlock with id " + std::to_string(id) + " exists.");
}

void ProgramBlockControlItemData::writeJsonObject(QJsonObject &o) {
    ControlItemData::writeJsonObject(o);
    if (program) o.insert("programBlock", QString::number(program->getID().value()));
}
void ProgramBlockControlItemData::setProgramBlock(Modules::ProgramBlock *p) {
    if (p != program) {
        QObject::disconnect(connection);
        program = p;
        if (program) {
            QQmlEngine::setObjectOwnership(program, QQmlEngine::CppOwnership);
            connection = QObject::connect(program, &QObject::destroyed, [this]() {
                static_cast<QQuickItem *>(parent())->setParentItem(nullptr);
                setProgramBlock(nullptr);
            });
        }
        emit programBlockChanged();
    }
}

/////////////////////////////////////
////////// ControlItemSync //////////
/////////////////////////////////////

ControlItemSync::ControlItemSync() {
    QObject::connect(&webSocket, qOverload<QAbstractSocket::SocketError>(&QWebSocket::error),
                     [this](const auto error) { qWarning() << "Remote ControlPane: WebSocket connection error: " << error << " : " << webSocket.errorString(); });
    QObject::connect(&webSocket, &QWebSocket::disconnected, [this]() {
        emit isConnectedChanged();
        // if we lost the connection, but don't want that, wait some time and reconnect then
        if (reconnectTimerId == -1) {
            reconnectTimerId = startTimer(WAIT_FOR_RECONNECT_MS);
        }
        killTimer(pingTimer);
    });
    QObject::connect(UserManagment::get(), &UserManagment::currentUserChanged, [this]() {
        if (isConnected()) {
            webSocket.sendTextMessage(QString("canMoveItems:") + (UserManagment::get()->getCurrentUser()->havePermission(UserManagment::MOVE_CONTROL_ITEM) ? "true" : "false"));
        }
    });
    QObject::connect(&webSocket, &QWebSocket::connected, [this]() {
        webSocket.sendTextMessage("name:" + this->settings->getComputerName());
        webSocket.sendTextMessage(QString("canMoveItems:") + (UserManagment::get()->getCurrentUser()->havePermission(UserManagment::MOVE_CONTROL_ITEM) ? "true" : "false"));
        for (const auto &ci : controlItems) {
            sendMessageForNewItem(ci);
        }
        emit isConnectedChanged();
        // if we were in reconnection mode, stop the timer
        if (reconnectTimerId >= 0) {
            killTimer(reconnectTimerId);
            reconnectTimerId = -1;
        }
        pingTimer = startTimer(PING_INTERVALL_MS);
    });
    QObject::connect(&webSocket, &QWebSocket::textMessageReceived, [this](const QString &message) {
        auto dot = message.indexOf('.');
        bool ok;
        auto id = message.left(dot).toInt(&ok);
        if (!ok) {
            qWarning() << "Message for ControlPane has wrong format: " << message;
            return;
        }
        auto callbacks = controlItemCallbacks.find(id);
        if (callbacks != controlItemCallbacks.end()) {
            auto colon = message.indexOf(':');
            auto property = message.mid(dot + 1, colon - dot - 1);
            auto callback = callbacks->second.find(property);
            if (callback != callbacks->second.end()) {
                callback->second(message.right(message.length() - colon - 1));
            } else {
                qWarning() << "Message for ControlPane: Unknown writeable property " << property << ". Message: " << message;
            }
        } else {
            qWarning() << "Message for ControlPane: Unknown component with id" << id << ". Message: " << message;
        }
    });
    connect();
}

void ControlItemSync::sendMessageForNewItem(ControlItemData *ci) {
    QString message = "add:{\"id\":" + QString::number(ci->id);
    ci->setInitialValuesForRemote([&](const QString &name, const QString &value) { message += ",\"" + name + "\" : \"" + value + "\""; });
    message += "}";
    webSocket.sendTextMessage(message);
}

void ControlItemSync::registerControlItenData(ControlItemData *data) {
    controlItems.push_back(data);
    controlItemCallbacks.insert({data->id, {}});
    if (isConnected()) {
        auto t = new QTimer();
        QObject::connect(t, &QTimer::timeout, [=]() {
            t->deleteLater();
            sendMessageForNewItem(data);
        });
        t->setSingleShot(true);
        t->setInterval(0);
        t->start(0);
    }
}

void ControlItemSync::deregisterControlItenData(ControlItemData *data) {
    controlItemCallbacks.erase(controlItemCallbacks.find(data->id));
    controlItems.erase(std::find(controlItems.cbegin(), controlItems.cend(), data));
    if (isConnected()) {
        webSocket.sendTextMessage("remove:" + QString::number(data->id));
    }
}

void ControlItemSync::setRemotePropertyChangeCallback(ControlItemData *data, const QString &name, std::function<void(const QString &)> callback) {
    controlItemCallbacks[data->id][name] = callback;
}

void ControlItemSync::propertyChangedForRemote(ControlItemData *data, const QString &name, const QString &value) {
    if (isConnected()) {
        webSocket.sendTextMessage(QString::number(data->id) + "." + name + ":" + value);
    }
}

void ControlItemSync::connect(Settings &settings) {
    this->settings = &settings;
    QObject::disconnect(computerNameConnection);
    connect();
    computerNameConnection = QObject::connect(&settings, &Settings::computerNameChanged, [this, &settings]() {
        if (isConnected()) {
            webSocket.sendTextMessage("name:" + settings.getComputerName());
        }
    });
}
void ControlItemSync::connect() {
    webSocket.open(QUrl(QStringLiteral("wss://orga.symposion.hilton.rwth-aachen.de/controlPaneClient")));
}

void ControlItemSync::timerEvent(QTimerEvent *event) {
    if (event->timerId() == reconnectTimerId) {
        event->accept();
        connect();
    } else if (event->timerId() == pingTimer) {
        event->accept();
        webSocket.ping();
    }
}

} // namespace GUI
