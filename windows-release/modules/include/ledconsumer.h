#ifndef LEDCONSUMER_H
#define LEDCONSUMER_H

#include "consumer.hpp"
#include "modelvector.h"
#include <QColor>
#include <QObject>
#include <cmath>

namespace Modules {

class LedConsumer;

class LedConsumer : public ModelVector<rgb_t>, public Consumer
{
    Q_OBJECT
    Q_PROPERTY(bool active MEMBER active NOTIFY activeChanged)
    Q_PROPERTY(QString name READ getNameFromUser NOTIFY nameChanged)
    StringProperty name;
    std::string lastName;
    int firstChangedIndex = 0;
    int changedLength = 0;
    int waitCounter = 0;
    bool active = false;
protected:
    void timerEvent(QTimerEvent*event) override;
public:
    inline static ModelVector<LedConsumer*> allLedConsumer;
    LedConsumer();
    ~LedConsumer()override{
        allLedConsumer.removeAll(this);
    }
    QString getNameFromUser()const{
        return QString::fromStdString(name.getString());
    }
    void setInputLength(unsigned int size) override{
        resize(size);
    }
    unsigned int getInputLength()const override{
        return static_cast<unsigned int>(size());
    }
    const char* getName()const override{
        return "Virtual LED Consumer";
    }

    void start() override {active = true;emit activeChanged();}
    void stop() override {active = false;emit activeChanged();}
    void show() override {}
    void doStep(time_diff_t diff) override;
    void setInputData(void*data, unsigned int index, unsigned int length) override;
    enum{
        RedDataRole = Qt::UserRole+1,
        GreenDataRole,
        BlueDataRole,
        ColorDataRole,
    };
    QHash<int,QByteArray> roleNames()const override;
    QVariant data(const QModelIndex &index, int role) const override;
signals:
    void activeChanged();
    void nameChanged();
};

} // namespace Modules

Q_DECLARE_METATYPE(Modules::rgb_t)


#endif // LEDCONSUMER_H
