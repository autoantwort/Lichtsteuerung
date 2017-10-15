#ifndef CHANNELPROGRAMMEDITOR_H
#define CHANNELPROGRAMMEDITOR_H

#include <QQuickItem>
#include <QFlags>
#include <programmprototype.h>

class ChannelProgrammEditor;

class CurrentTimePointWrapper : public QObject{
    Q_OBJECT
    Q_PROPERTY(unsigned char value READ getValue WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(bool hasCurrent READ hasCurrent NOTIFY hasCurrentChanged)
    Q_PROPERTY(double time READ getTime WRITE setTime NOTIFY timeChanged)
    Q_PROPERTY(QEasingCurve curveToNext READ getCurve WRITE setCurve NOTIFY curveChanged)
    ChannelProgrammEditor * editor;
public:
    CurrentTimePointWrapper(ChannelProgrammEditor * editor);
    void setValue(unsigned char value);
    unsigned char getValue()const;
    bool hasCurrent();
    void setTime(double time);
    double getTime()const;
    void setCurve(const QEasingCurve &c);
    QEasingCurve getCurve()const;
signals:
    void valueChanged();
    void hasCurrentChanged();
    void timeChanged();
    void curveChanged();
};

class ChannelProgrammEditor : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(CurrentTimePointWrapper * currentTimePoint READ getCurrentTimePointWrapper CONSTANT)
    Q_PROPERTY(QQuickItem* background READ getBackgroundItem WRITE setBackgroundItem NOTIFY backgroundItemChanged)
    Q_PROPERTY(QQuickItem* informationDisplayTextItem READ getValueChangeItem WRITE setValueChangeItem NOTIFY valueChangeItemChanged)
    Q_PROPERTY(int currentChangingValue READ getCurrentChangingValue NOTIFY currentChangingValueChanged)
    Q_PROPERTY(int clickRadius MEMBER clickRadius NOTIFY clickRadiusChanged)
    Q_PROPERTY(QColor graphColor MEMBER graphColor NOTIFY graphColorChanged)
    Q_PROPERTY(QString tooltipText READ getTooltipText CONSTANT)
    Q_PROPERTY(ChannelProgramm* channelProgramm READ getChannelProgramm WRITE setChannelProgramm NOTIFY channelProgrammChanged)
private:
    CurrentTimePointWrapper currentTimePointWrapper;
    ChannelProgramm * channelProgramm = nullptr;
    QQuickItem * backgroundItem = nullptr;
    QQuickItem * valueChangeItem = nullptr;
    decltype(ChannelProgramm::timeline)::iterator currentTimePoint;
    int clickRadius=4;
    double xScale = 1.0;
    QColor graphColor = QColor(0,0,0);
    enum Modifier{X_PRESSED,Y_PRESSED,N_PRESSED,T_PRESSED,V_PRESSED,D_PRESSED};
    QFlags<Modifier> modifier;
    decltype(ChannelProgramm::timeline)::iterator getTimePointForPosition(int x, int y);
    double getScaledX(int x){return x * 1/xScale;}
    int getScaledY(int y){return 255 - int(y / height() * 255);}
public:
    ChannelProgrammEditor();
    void setBackgroundItem(QQuickItem*b);
    QQuickItem* getBackgroundItem()const{return backgroundItem;}
    void setValueChangeItem(QQuickItem*b);
    QQuickItem* getValueChangeItem()const{return valueChangeItem;}
    int getCurrentChangingValue()const{return haveCurrentTimePoint()?currentTimePoint->value:0;}
    void setChannelProgramm(ChannelProgramm*p);
    ChannelProgramm* getChannelProgramm()const{return channelProgramm;}
    QString getTooltipText()const;
    bool haveCurrentTimePoint()const{return channelProgramm?(currentTimePoint!=channelProgramm->timeline.cend()):false;}
    TimePoint * getCurrentTimePoint();
    CurrentTimePointWrapper * getCurrentTimePointWrapper(){return &currentTimePointWrapper;}
protected:
    virtual void updatePolish()override;
    virtual QSGNode* updatePaintNode(QSGNode *, UpdatePaintNodeData *)override;
    virtual void mouseMoveEvent(QMouseEvent *event)override;
    virtual void mousePressEvent(QMouseEvent *event)override;
    virtual void mouseReleaseEvent(QMouseEvent *event)override;
    virtual void keyPressEvent(QKeyEvent *event)override;
    virtual void keyReleaseEvent(QKeyEvent *event)override;
signals:
    void backgroundItemChanged();
    void valueChangeItemChanged();
    void currentChangingValueChanged(int);
    void clickRadiusChanged();
    void graphColorChanged();
    void channelProgrammChanged();
};

#endif // CHANNELPROGRAMMEDITOR_H
