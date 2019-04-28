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
    Q_PROPERTY(QEasingCurve::Type curveToNext READ getCurve WRITE setCurve NOTIFY curveChanged)
    ChannelProgrammEditor * editor;
public:
    CurrentTimePointWrapper(ChannelProgrammEditor * editor);
    void setValue(unsigned char value);
    unsigned char getValue()const;
    bool hasCurrent();
    void setTime(double time);
    double getTime()const;
    void setCurve(const QEasingCurve::Type &c);
    QEasingCurve::Type getCurve()const;
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
    friend class CurrentTimePointWrapper;
    CurrentTimePointWrapper currentTimePointWrapper;
    ChannelProgramm * channelProgramm = nullptr;
    QQuickItem * backgroundItem = nullptr;
    QQuickItem * valueChangeItem = nullptr;
    decltype(ChannelProgramm::timeline)::iterator currentTimePoint;
    int clickRadius=4;
    double xScale = 4.;
    double totalXOffset = 0;
    double xDiff = 0 ;
    QColor graphColor = QColor(0,0,0);
    enum Modifier{X_PRESSED=0x1,Y_PRESSED=0x2,N_PRESSED=0x4,T_PRESSED=0x8,V_PRESSED=0x10,D_PRESSED=0x20};
    QFlags<Modifier> modifier;
#define INVALID_POS QPoint(std::numeric_limits<int>::max(),std::numeric_limits<int>::max())
    QPoint lastMousePosition = INVALID_POS;
    ulong mousePressTimestamp;
    decltype(ChannelProgramm::timeline)::iterator getTimePointForPosition(int x, int y);
    /**
     * @brief mapToVisualX maps from values/mouse Position to the visual Position of the graph
     * eg: mouse/value = 100
     * xScale = 2
     * totalXOffset = 100
     * result = x*xScale+totalXOffset = 300
     * @param x the x coordinate
     * @return x * xScale + totalXOffset
     */
    double mapToVisualX(double x){return x * xScale + totalXOffset;}
    /**
     * @brief mapFromVisualX maps from the visual Position of the graph to real values
     * @param xthe x coordinate
     * eg: visual value = 300
     * xScale = 2
     * totalXOffset = 100
     * result = (x-totalXOffset)*(1/xScale) = 100
     * @return
     */
    double mapFromVisualX(double x){return (x-totalXOffset) * (1/xScale);}
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
    Q_INVOKABLE QEasingCurve * getCurveForPoint(int x);
protected:
    virtual void updatePolish()override;
    virtual void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)override;
    virtual QSGNode* updatePaintNode(QSGNode *, UpdatePaintNodeData *)override;
    virtual void mouseMoveEvent(QMouseEvent *event)override;
    virtual void mousePressEvent(QMouseEvent *event)override;
    virtual void mouseReleaseEvent(QMouseEvent *event)override;
    virtual void hoverEnterEvent(QHoverEvent*e)override{lastMousePosition=e->pos();e->accept();forceActiveFocus(Qt::MouseFocusReason);}
    virtual void hoverMoveEvent(QHoverEvent*e)override;
    virtual void hoverLeaveEvent(QHoverEvent*e)override{lastMousePosition=INVALID_POS;update();e->accept();}
    virtual void keyPressEvent(QKeyEvent *event)override;
    virtual void keyReleaseEvent(QKeyEvent *event)override;
    virtual void wheelEvent(QWheelEvent *event)override;
signals:
    void backgroundItemChanged();
    void valueChangeItemChanged();
    void currentChangingValueChanged(int);
    void clickRadiusChanged();
    void graphColorChanged();
    void channelProgrammChanged();
    void click(int x, int y);
    void rightClick(int x, int y);
};

#endif // CHANNELPROGRAMMEDITOR_H
