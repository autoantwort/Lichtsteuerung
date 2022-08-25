#ifndef CONTROLITEM_H
#define CONTROLITEM_H

#include "controlitemdata.h"
#include <QQuickItem>

namespace GUI {

/**
 * @brief The ControlItem class is the base for ControlItems like the SwitchControlItem or the DimmerControlItem
 */
class ControlItem : public QQuickItem {
    Q_OBJECT
    Q_PROPERTY(int rasterSize MEMBER rasterSize NOTIFY rasterSizeChanged)
    Q_PROPERTY(int blockWidth READ getBlockWidth WRITE setBlockWidth NOTIFY blockWidthChanged)
    Q_PROPERTY(int blockHeight READ getBlockHeight WRITE setBlockHeight NOTIFY blockHeightChanged)
    Q_PROPERTY(ControlItemData *controlData READ getControlData WRITE setControlData NOTIFY controlDataChanged)
    Q_PROPERTY(bool moveable READ isMoveable WRITE setMoveable NOTIFY moveableChanged)
private:
    /**
     * @brief data The Data that a spezific ControlItem needs are saved here
     */
    ControlItemData *data = nullptr;
    int blockWidth = 0;
    int blockHeight = 0;
    int rasterSize = 50;
    bool moveable = false;
    QPoint grabPos;
    QPoint startPos;

public:
    ControlItem();
    void setBlockWidth(int width);
    void setBlockHeight(int height);
    void setMoveable(bool m);
    int getBlockWidth() const { return blockWidth; }
    int getBlockHeight() const { return blockHeight; }
    bool isMoveable() const { return moveable; }
    void setControlData(ControlItemData *);
    ControlItemData *getControlData() const { return data; }
    void savePositionInControlData();

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void itemChange(ItemChange change, const ItemChangeData &value);
    virtual void hoverEnterEvent(QHoverEvent *event);
    virtual void hoverMoveEvent(QHoverEvent *event);
    virtual void hoverLeaveEvent(QHoverEvent *event);

signals:
    void rasterSizeChanged();
    void blockWidthChanged();
    void blockHeightChanged();
    /**
     * @brief settingVisibleChange when the mouse enter/leaves the upper right corner of the Item, this signal is shot
     * @param visible true if the Settings symbol should be visible
     */
    void settingVisibleChange(bool visible);
    /**
     * @brief openPopup this signal is given, when the user rightclick on the Item
     * @param x the x position of the Mouse
     * @param y the y position of the Mouse
     */
    void openPopup(int x, int y);
    void controlDataChanged();
    void moveableChanged();
public slots:
};

} // namespace GUI

#endif // CONTROLITEM_H
