#ifndef CONTROLITEMDATA_H
#define CONTROLITEMDATA_H

#include <QObject>
#include "dmx/programm.h"
#include "dmx/device.h"
#include "modules/programblock.h"
#include "modelmanager.h"

namespace GUI{

class UserVisibilityModel : public QAbstractListModel{
    Q_OBJECT
    std::set<ID> excudedUsers;
public:
    UserVisibilityModel() = default;
    UserVisibilityModel(const QJsonObject&);
    void writeJsonObject(QJsonObject &o)const;
    bool isVisibleForCurrentUser()const;
    enum UserVisibilityModelRoles{
        UserNameRole = Qt::UserRole+1,
        VisibilityRole
    };
    virtual int rowCount(const QModelIndex & = QModelIndex())const override {return UserManagment::get()->getUsers().ssize();}
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole)override;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override{
        auto r = QAbstractListModel::flags(index);
        r.setFlag(Qt::ItemIsEditable);
        return r;
    }
    QHash<int,QByteArray> roleNames() const override{
        auto r = QAbstractListModel::roleNames();
        r[UserNameRole] = "userName";
        r[VisibilityRole] = "isVisible";
        return r;
    }
};

class ControlItemData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int startXBlock READ getStartXBlock WRITE setStartXBlock NOTIFY startXBlockChanged)
    Q_PROPERTY(int startYBlock READ getStartYBlock WRITE setStartYBlock NOTIFY startYBlockChanged)
    Q_PROPERTY(bool isVisibleForUser READ isVisibleForUser NOTIFY isVisibleForUserChanged)
    Q_PROPERTY(QAbstractListModel* userVisibilityModel READ getUserVisibilityModel CONSTANT)
private:
    int startXBlock=0;
    int startYBlock=0;
    UserVisibilityModel userVisibilityModel;
public:
    enum Type{PROGRAMM, SWITCH_GROUP, DIMMER_GROUP, PROGRAM_BLOCK};
private:
    Type type;
public:
    ControlItemData() = default;
    virtual ~ControlItemData() = default;
    explicit ControlItemData(Type t, QObject *parent = nullptr);
    ControlItemData(const QJsonObject &o, QObject *parent = nullptr);
    virtual void writeJsonObject(QJsonObject &o);
    int getStartXBlock()const{return startXBlock;}
    int getStartYBlock()const{return startYBlock;}
    void setStartXBlock(int);
    void setStartYBlock(int);
    QAbstractListModel* getUserVisibilityModel(){return &userVisibilityModel;}
    bool isVisibleForUser()const{return userVisibilityModel.isVisibleForCurrentUser();}
signals:
    void startXBlockChanged();
    void startYBlockChanged();
    void isVisibleForUserChanged();
};

class ProgrammControlItemData : public ControlItemData{
    Q_OBJECT
    Q_PROPERTY(DMX::Programm* programm READ getProgramm WRITE setProgramm NOTIFY programmChanged)
    DMX::Programm * programm=nullptr;
public:
    ProgrammControlItemData(DMX::Programm * p,QObject *parent = nullptr);
    ProgrammControlItemData(const QJsonObject &o,QObject *parent = nullptr);
    virtual void writeJsonObject(QJsonObject &o);
    void setProgramm(DMX::Programm * );
    DMX::Programm * getProgramm()const{return programm;}
signals:
    void programmChanged();
};

class GroupModel : public QAbstractListModel{
    Q_OBJECT
    QAbstractItemModel * deviceModel;
    std::vector<bool> enabled;
protected:
    void handleRowsAboutToBeInserted(const QModelIndex &parent, int start, int end);
    void handleRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last);
    void handleRowsInserted(const QModelIndex &parent, int first, int last);
    void handleRowsRemoved(const QModelIndex &parent, int first, int last);
public:
    GroupModel(QAbstractItemModel * deviceModel = ModelManager::get().getDeviceModel());
    enum GroupModelRoles{
        EnableDataRole = ModelVector<std::unique_ptr<DMX::Device>>::ModelDataRole + 1
    };
    const std::vector<bool>& getEnabledVector()const{return enabled;}
    void setEnabled(int index, bool enabled = true){
        if(index>=0 && index<rowCount(QModelIndex())){
            this->enabled[index] = enabled;
        }
    }
    virtual int rowCount(const QModelIndex &parent = QModelIndex())const override {return deviceModel->rowCount(parent);}
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)const override{
        if(role == EnableDataRole){
            if(index.row()>=0 && index.row()<rowCount(index)){
                return QVariant(enabled.at(index.row()));
            }else{
                qDebug()<<"index out of range row x colum : "<<index.row()<<' '<<index.column();
            }
            return QVariant("Index Out of Range");
        }else{
            return deviceModel->data(index,role);
        }
        return QVariant("Keine EnableDataRole");
    }
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole)override{
        if(role==EnableDataRole){
            if(index.row()>=0 && index.row()<rowCount(index)){
                if(enabled[index.row()] != value.toBool()){
                    enabled[index.row()] = value.toBool();
                    emit dataChanged(index,index,{EnableDataRole});
                    return true;
                }
            }
        }
        return false;
    }
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override{
        auto r = deviceModel->flags(index);
        r.setFlag(Qt::ItemIsEditable);
        return r;
    }
    QHash<int,QByteArray> roleNames() const override{
        auto r = deviceModel->roleNames();
        r[EnableDataRole] = "use";
        return r;
    }
};

class GroupControlItemData : public ControlItemData{
    Q_OBJECT
    Q_PROPERTY(GroupModel* groupModel READ getGroupModel CONSTANT)
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
    GroupModel groupModel;
    QString name;
public:
    GroupControlItemData(ControlItemData::Type t, QObject *parent = nullptr):ControlItemData(t,parent){}
    GroupControlItemData(const QJsonObject &o,QObject *parent = nullptr);
    virtual void writeJsonObject(QJsonObject &o);
    GroupModel * getGroupModel(){return &groupModel;}
    void forEach(std::function<void(DMX::Device*)> f);

    void setName(QString n);
    QString getName()const{return name;}
signals:
    void nameChanged();
};

class SwitchGroupControlItemData : public GroupControlItemData{
    Q_OBJECT
    Q_PROPERTY(bool activated READ isActivated WRITE setActivated NOTIFY activatedChanged)
    Q_PROPERTY(int activateCooldown READ getActivateCooldown WRITE setActivateCooldown NOTIFY activateCooldownChanged)
    Q_PROPERTY(int deactivateCooldown READ getDeactivateCooldown WRITE setDeactivateCooldown NOTIFY deactivateCooldownChanged)

    bool activated = false;
    int activateCooldown = 1000;
    int deactivateCooldown = 1000;
public:
    SwitchGroupControlItemData(QObject *parent = nullptr):GroupControlItemData(SWITCH_GROUP,parent){}
    SwitchGroupControlItemData(const QJsonObject &o,QObject *parent = nullptr);
    virtual void writeJsonObject(QJsonObject &o);
    void setActivated(bool a);
    bool isActivated()const{return activated;}
    void setActivateCooldown(int c);
    void setDeactivateCooldown(int c);
    int getActivateCooldown()const{return activateCooldown;}
    int getDeactivateCooldown()const{return deactivateCooldown;}
signals:
    void activatedChanged();
    void activateCooldownChanged();
    void deactivateCooldownChanged();
};

class DimmerGroupControlItemData : public GroupControlItemData{
    Q_OBJECT
    Q_PROPERTY(Operation maxOperation READ getMaxOperation WRITE setMaxOperation NOTIFY maxOperationChanged)
    Q_PROPERTY(Operation minOperation READ getMinOperation WRITE setMinOperation NOTIFY minOperationChanged)
    Q_PROPERTY(unsigned char maxValue READ getMaxValue WRITE setMaxValue NOTIFY maxValueChanged)
    Q_PROPERTY(unsigned char minValue READ getMinValue WRITE setMinValue NOTIFY minValueChanged)
    Q_PROPERTY(unsigned char value READ getValue WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(bool override READ shouldOverrideValue WRITE shouldOverrideValue NOTIFY shouldOverrideValueChanged)
    typedef DMX::DMXChannelFilter::Operation Operation;
    Operation maxOperation = Operation::CUT;
    Operation minOperation = Operation::CUT;
    unsigned char maxValue = 255;
    unsigned char minValue = 0;
    unsigned char value = 0;
    bool shouldOverrideValue_;
public:
    DimmerGroupControlItemData(QObject *parent = nullptr):GroupControlItemData(DIMMER_GROUP,parent){}
    DimmerGroupControlItemData(const QJsonObject &o);
    virtual void writeJsonObject(QJsonObject &o);

    Q_SLOT void setMaxOperation(Operation o);
    Operation getMaxOperation()const{return maxOperation;}
    Q_SLOT void setMinOperation(Operation o);
    Operation getMinOperation()const{return minOperation;}

    Q_SLOT void setMaxValue(unsigned char o);
    unsigned char getMaxValue()const{return maxValue;}
    Q_SLOT void setMinValue(unsigned char o);
    unsigned char getMinValue()const{return minValue;}

    Q_SLOT void setValue(unsigned char o);
    unsigned char getValue()const{return value;}

    Q_SLOT void shouldOverrideValue(bool o);
    bool shouldOverrideValue()const{return shouldOverrideValue_;}


signals:
    void maxOperationChanged();
    void minOperationChanged();
    void maxValueChanged();
    void minValueChanged();
    void valueChanged();
    void shouldOverrideValueChanged();
};

class ProgramBlockControlItemData : public ControlItemData{
    Q_OBJECT
    Q_PROPERTY(Modules::ProgramBlock* programBlock READ getProgramBlock WRITE setProgramBlock NOTIFY programBlockChanged)
    Modules::ProgramBlock * program=nullptr;
    QMetaObject::Connection connection;
public:
    ProgramBlockControlItemData(Modules::ProgramBlock * p,QObject *parent = nullptr);
    ProgramBlockControlItemData(const QJsonObject &o,QObject *parent = nullptr);
    ~ProgramBlockControlItemData()override{QObject::disconnect(connection);}
    virtual void writeJsonObject(QJsonObject &o)override;
    void setProgramBlock(Modules::ProgramBlock * );
    Modules::ProgramBlock * getProgramBlock()const{return program;}
signals:
    void programBlockChanged();
};

} // namespace GUI

#endif // CONTROLITEMDATA_H
