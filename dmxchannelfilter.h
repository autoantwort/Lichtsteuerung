#ifndef DMXCHANNEL_H
#define DMXCHANNEL_H

#include <QObject>

class DMXChannelFilter : QObject{
    Q_OBJECT
public:
    enum Operation{CUT, REMAP};
    Q_ENUM(Operation)
private:
    Operation maxOperation = CUT;
    Operation minOperation = CUT;
    unsigned char maxValue = 255;
    unsigned char minValue = 0;
    unsigned char value = 0;
    bool shouldOverrideValue_;
public:
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

    Q_SLOT void shouldOverrideValue(unsigned char o){if(o==shouldOverrideValue_)return;shouldOverrideValue_=o;emit shouldOverrideValueChanged(o);}
    unsigned char shouldOverrideValue()const{return shouldOverrideValue_;}

public:
    void initValue(unsigned char * value);
    void filterValue(unsigned char * value);
public:
    static void initValues(unsigned char *values, unsigned int numberOfChannels);
    static void filterValues(unsigned char *values, unsigned int numberOfChannels);
signals:
    void maxOperationChanged(Operation);
    void minOperationChanged(Operation);
    void maxValueChanged(unsigned char);
    void minValueChanged(unsigned char);
    void valueChanged(unsigned char);
    void shouldOverrideValueChanged(unsigned char);
};

#endif // DMXCHANNEL_H