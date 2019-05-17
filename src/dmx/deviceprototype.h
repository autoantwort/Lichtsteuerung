#ifndef DEVICEPROTOTYPE_H
#define DEVICEPROTOTYPE_H

#include <QString>
#include <vector>
#include <QVector>
#include "channel.h"
#include "idbase.h"

namespace DMX{

class ChannelVector : public QAbstractListModel{
    std::vector<Channel*> channels;
public:
    enum{
        NameRole = Qt::UserRole+1,IndexRole,DescriptionRole
    };
    const std::vector<Channel*>& getChannel()const{return channels;}
    std::vector<Channel*>& getChannel(){return channels;}
    virtual int rowCount(const QModelIndex &) const override{return static_cast<int>(channels.size());}
    virtual QVariant data(const QModelIndex &index, int role) const override{
        if(index.row()>=0&&index.row()<int(channels.size())){
            switch(role){
            case Qt::DisplayRole:
            case NameRole:
                return channels[index.row()]->getName();
            case Qt::ToolTipRole:
            case DescriptionRole:
                return channels[index.row()]->getDescription();
            case IndexRole:
                return channels[index.row()]->getIndex();
            }
            return QVariant::fromValue(channels[index.row()]);
        }
        qDebug()<<"index out\n";
        return QVariant();
    }
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override{
        if(index.row()>=0&&index.row()<int(channels.size())){
            switch(role){
            case Qt::DisplayRole:
            case NameRole:
                 channels[index.row()]->setName(value.toString());
                break;
            case Qt::ToolTipRole:
            case DescriptionRole:
                 channels[index.row()]->setDescription(value.toString());
                break;
            default:
                return false;
            }
            return true;
        }
        return false;
    }
    QHash<int,QByteArray> roleNames()const override{
        QHash<int,QByteArray> r;
        r[NameRole] = "name";
        r[IndexRole] = "index";
        r[DescriptionRole] = "description";
        return r;
    }
    void pop_back();
    void beginPushBack(int length);
    void endPushBack();
};

/** Jedes Gerät(Scanner/Laser/Lampe/...) bekommt ein Device Prototype, wo festgelegt wird, auf welchem Channel welche Daten anliegen
 * @brief The DevicePrototype class
 */
class DevicePrototype : public NamedObject, public IDBase<DevicePrototype>
{
    Q_OBJECT
    Q_PROPERTY(int numberOfChannels READ getNumberOfChannels NOTIFY numberOfChannelsChanged)
    Q_PROPERTY(QAbstractItemModel * channel READ getChannelModel CONSTANT)
private:
    /**
     * @brief channels Ein vector die die einzelnen Channel Informationen hält
     */
    ChannelVector channels;
public:
    DevicePrototype(const QJsonObject &o);
    DevicePrototype(QString name, QString description=""):NamedObject(name,description){}
    int getNumberOfChannels()const{return static_cast<int>(channels.getChannel().size());}
    /**
     * @brief removeChannels Entfernt Channel bis zu einem bestimmten Index
     * @param newMaxIndex Der neue Maximale Index (Inclusiv)
     */
    void removeChannels(int newMaxIndex);
    /**
     * @brief popChannel Entfernt den letzten Channel
     */
    Q_INVOKABLE void popChannel(){removeChannels(getNumberOfChannels()-1);}
    Q_INVOKABLE void pushChannel(QString name, QString description=""){addChannel(getNumberOfChannels(),name,description);}
    /**
     * @brief addChannelDescription Setzt ein Channel neu oder fügt einen neuen Channel mit dem jeweiligen Channel hinzu
     * Wenn eine Lücke zwischen den bisherigen maximalen Index und dem neuen Index liegt, wird diese mit Dummy Channeln aufgefüllt.
     * @param channel Der Index des Channels, darf größer als getNumberOfChannels Sein.
     * @param description die Beschreibung des Channels.
     */
    void addChannel(int channel, QString name, QString description="");

    const Channel * getChannelByName(const QString &name)const;
    const Channel * getChannelById(const int id)const;
    const Channel * getChannelByIndex(const unsigned int channelIndex)const;

    const std::vector<Channel*> & getChannels()const{return channels.getChannel();}

    void writeJsonObject(QJsonObject &o)const;

    ChannelVector * getChannelModel(){return &channels;}
    const ChannelVector * getChannelModel()const{return &channels;}

signals:
    void numberOfChannelsChanged();
    void channelAdded(Channel*);
    void channelRemoved(Channel*);

};

} // namespace DMX

#endif // DEVICEPROTOTYPE_H