#ifndef DMXCHANNEL_H
#define DMXCHANNEL_H

#include <QObject>

/**
 * @brief The DMXChannelFilter class filter channel values
 * Die Klasse kann einen Default value, einen Override value (überschreibt den value vom Programmen)
 * einen min und einen max value festlegen und methoden angeben, wie mit den beiden werten umgegangen
 * werden soll
 */
class DMXChannelFilter : public QObject{
    Q_OBJECT
public:
    /**
     * @brief The Operation enum legt die Operationen für den min und max value fest
     * CUT : Scheident die werte einfach ab
     * REMAP : die werte werden auf den neuen bereich skaliert
     */
    enum Operation{CUT, REMAP};
    Q_ENUM(Operation)

private:
    Operation maxOperation = CUT;
    Operation minOperation = CUT;
    unsigned char maxValue = 255;
    unsigned char minValue = 0;
    unsigned char value = 0;
    bool shouldOverrideValue_ = false;
public:
    DMXChannelFilter() = default;
    DMXChannelFilter(const QJsonObject &o);


    void writeJsonData(QJsonObject & o);

    Q_SLOT void setMaxOperation(Operation o){if(o==maxOperation)return;maxOperation=o;emit maxOperationChanged(o);}
    Operation getMaxOperation()const{return maxOperation;}
    Q_SLOT void setMinOperation(Operation o){if(o==minOperation)return;minOperation=o;emit minOperationChanged(o);}
    Operation getMinOperation()const{return minOperation;}

    Q_SLOT void setMaxValue(unsigned char o){if(o==maxValue)return;maxValue=o;emit maxValueChanged(o);}
    unsigned char getMaxValue()const{return maxValue;}
    Q_SLOT void setMinValue(unsigned char o){if(o==minValue)return;minValue=o;emit minValueChanged(o);}
    unsigned char getMinValue()const{return minValue;}

    Q_SLOT void setValue(unsigned char o){if(o==value)return;value=o;emit valueChanged(o);}
    unsigned char getValue()const{return value;}

    Q_SLOT void shouldOverrideValue(bool o){if(o==shouldOverrideValue_)return;shouldOverrideValue_=o;emit shouldOverrideValueChanged(o);}
    bool shouldOverrideValue()const{return shouldOverrideValue_;}

public:
    void initValue(unsigned char * value);
    void filterValue(unsigned char * value);
public:
    /**
     * @brief initValues sets the default values for every channel
     * @param values the values
     * @param numberOfChannels the number of channels
     */
    static void initValues(unsigned char *values, unsigned int numberOfChannels);
    /**
     * @brief filterValues filters the values that are generated from the programms
     * @param values
     * @param numberOfChannels
     */
    static void filterValues(unsigned char *values, unsigned int numberOfChannels);
signals:
    void maxOperationChanged(Operation);
    void minOperationChanged(Operation);
    void maxValueChanged(unsigned char);
    void minValueChanged(unsigned char);
    void valueChanged(unsigned char);
    void shouldOverrideValueChanged(bool);
};

#endif // DMXCHANNEL_H
