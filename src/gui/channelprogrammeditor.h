#ifndef CHANNELPROGRAMMEDITOR_H
#define CHANNELPROGRAMMEDITOR_H

#include <QFlags>
#include <QQuickItem>
#include <dmx/programmprototype.h>

namespace GUI {

class ChannelProgrammEditor;

class CurrentTimePointWrapper : public QObject {
    Q_OBJECT
    Q_PROPERTY(unsigned char value READ getValue WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(bool hasCurrent READ hasCurrent NOTIFY hasCurrentChanged)
    Q_PROPERTY(double time READ getTime WRITE setTime NOTIFY timeChanged)
    ChannelProgrammEditor *editor;

public:
    CurrentTimePointWrapper(ChannelProgrammEditor *editor);
    void setValue(unsigned char value);
    unsigned char getValue() const;
    bool hasCurrent();
    void setTime(double time);
    double getTime() const;
signals:
    void valueChanged();
    void hasCurrentChanged();
    void timeChanged();
};

class ChannelProgrammEditor : public QQuickItem {
    Q_OBJECT
    Q_PROPERTY(CurrentTimePointWrapper *currentTimePoint READ getCurrentTimePointWrapper CONSTANT)
    Q_PROPERTY(int clickRadius MEMBER clickRadius NOTIFY clickRadiusChanged)
    Q_PROPERTY(QColor graphColor MEMBER graphColor NOTIFY graphColorChanged)
    Q_PROPERTY(DMX::ChannelProgramm *channelProgramm READ getChannelProgramm WRITE setChannelProgramm NOTIFY channelProgrammChanged)
    Q_PROPERTY(double hoverSegmentX READ getHoverSegmentX NOTIFY hoverSegmentXChanged)
    Q_PROPERTY(double hoverSegmentLength READ getHoverSegmentLength NOTIFY hoverSegmentLengthChanged)
    Q_PROPERTY(double selectedSegmentX READ getSelectedSegmentX NOTIFY selectedSegmentXChanged)
    Q_PROPERTY(double selectedSegmentLength READ getSelectedSegmentLength NOTIFY selectedSegmentLengthChanged)
    Q_PROPERTY(QEasingCurve::Type selectedEasingCurve READ getSelectedEasingCurve WRITE setSelectedEasingCurve NOTIFY selectedEasingCurveChanged)
    Q_PROPERTY(double mouseX READ getMouseX NOTIFY mouseXChanged)
private:
    friend class CurrentTimePointWrapper;
    CurrentTimePointWrapper currentTimePointWrapper;
    DMX::ChannelProgramm *channelProgramm = nullptr;
    decltype(DMX::ChannelProgramm::timeline)::iterator currentTimePoint;
    decltype(DMX::ChannelProgramm::timeline)::iterator currentSegment;
    int clickRadius = 4;
    double xScale = 50.;
    double totalXOffset = 0;
    QColor graphColor = QColor(0, 0, 0);
    enum Modifier { X_PRESSED = 0x1, Y_PRESSED = 0x2, N_PRESSED = 0x4, D_PRESSED = 0x20 };
    QFlags<Modifier> modifier;

    double hoverSegmentX = 0;
    double hoverSegmentLength = 0;
    void updateHoverSegment();
    double selectedSegmentX = 0;
    double selectedSegmentLength = 0;
    void updateSelectedSegment();

    static constexpr auto INVALID_POS = QPointF(std::numeric_limits<qreal>::lowest(), std::numeric_limits<qreal>::lowest());
    QPointF lastMousePosition = INVALID_POS;
    ulong mousePressTimestamp;
    decltype(DMX::ChannelProgramm::timeline)::iterator getTimePointForPosition(int x, int y);
    /**
     * @brief mapToVisualX maps from values/mouse Position to the visual Position of the graph
     * eg: mouse/value = 100
     * xScale = 2
     * totalXOffset = 100
     * result = x*xScale+totalXOffset = 300
     * @param x the x coordinate
     * @return x * xScale + totalXOffset
     */
    double mapToVisualX(double x) const { return x * xScale + totalXOffset; }
    /**
     * @brief mapFromVisualX maps from the visual Position of the graph to real values
     * @param xthe x coordinate
     * eg: visual value = 300
     * xScale = 2
     * totalXOffset = 100
     * result = (x-totalXOffset)*(1/xScale) = 100
     * @return
     */
    double mapFromVisualX(double x) const { return (x - totalXOffset) * (1 / xScale); }
    int getScaledY(int y) const { return 255 - int(y / height() * 255); }

public:
    ChannelProgrammEditor();
    void setChannelProgramm(DMX::ChannelProgramm *p);
    DMX::ChannelProgramm *getChannelProgramm() const { return channelProgramm; }
    bool haveCurrentTimePoint() const { return channelProgramm ? (currentTimePoint != channelProgramm->timeline.cend()) : false; }
    DMX::TimePoint *getCurrentTimePoint();
    CurrentTimePointWrapper *getCurrentTimePointWrapper() { return &currentTimePointWrapper; }
    Q_INVOKABLE QEasingCurve *getCurveForPoint(int x);
    double getHoverSegmentX() const { return hoverSegmentX; }
    double getHoverSegmentLength() const { return hoverSegmentLength; }
    double getSelectedSegmentX() const { return selectedSegmentX; }
    double getSelectedSegmentLength() const { return selectedSegmentLength; }
    QEasingCurve::Type getSelectedEasingCurve() const { return channelProgramm && currentSegment != channelProgramm->timeline.end() ? currentSegment->easingCurveToNextPoint.type() : static_cast<QEasingCurve::Type>(-1); }
    void setSelectedEasingCurve(QEasingCurve::Type type);
    double getMouseX() const { return lastMousePosition.x(); }
    Q_INVOKABLE double getTimeForVisualX(double x) const { return mapFromVisualX(x); }
    Q_INVOKABLE int getValueForVisualX(double x) const { return channelProgramm ? channelProgramm->getValueForTime(std::max(0., mapFromVisualX(x))) : -1; }

protected:
    virtual QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void hoverEnterEvent(QHoverEvent *e) override;
    virtual void hoverMoveEvent(QHoverEvent *e) override;
    virtual void hoverLeaveEvent(QHoverEvent *e) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;
signals:
    void clickRadiusChanged();
    void graphColorChanged();
    void channelProgrammChanged();
    void click(int x, int y);
    void rightClick(int x, int y);
    void hoverSegmentXChanged();
    void hoverSegmentLengthChanged();
    void selectedSegmentXChanged();
    void selectedSegmentLengthChanged();
    void selectedEasingCurveChanged();
    void mouseXChanged();
};

} // namespace GUI

#endif // CHANNELPROGRAMMEDITOR_H
