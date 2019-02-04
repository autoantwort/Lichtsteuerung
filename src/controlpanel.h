#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QQuickItem>
#include "programm.h"

/**
 * @brief The ControlPanel is the Component, that holds multiple ControlItems
 */
class ControlPanel : public QQuickItem
{
    Q_OBJECT
    static ControlPanel * lastCreated;
    static QQmlEngine * engine;
    Q_PROPERTY(int menuHeight READ getMenuHeight WRITE setMenuHeight NOTIFY menuHeightChanged)
    Q_PROPERTY(int menuWidth READ getMenuWidth WRITE setMenuWidth NOTIFY menuWidthChanged)
    QQmlComponent programm;
    QQmlComponent switchGroup;
    QQmlComponent dimmerGroup;
    int menuHeight;
    int menuWidth;
public:
    ControlPanel();
    static ControlPanel * getLastCreated(){return lastCreated;}
    static void setQmlEngine(QQmlEngine * e){engine=e;}
    void loadFromJsonObject(const QJsonObject &o);
    void writeJsonObject(QJsonObject &o);
    Q_INVOKABLE void addProgrammControl(Programm * programm);
    /**
     * @brief addSwitchGroupControl add a SwitchControlItem to enable and disable Devices to the panel
     */
    Q_INVOKABLE void addSwitchGroupControl();
    /**
     * @brief addDimmerGroupControl add a DimmerControlItem to the panel that set the channel values from 0 to 255 of the first channel of the selected Devices
     */
    Q_INVOKABLE void addDimmerGroupControl();
    int getMenuHeight()const{return menuHeight;}
    int getMenuWidth ()const{return menuWidth;}
    void setMenuHeight(int i){if(i!=menuHeight){menuHeight=i;emit menuHeightChanged();}}
    void setMenuWidth (int i){if(i!=menuWidth ){menuWidth =i;emit menuWidthChanged ();}}
signals:
    void menuHeightChanged();
    void menuWidthChanged();
    void exitMenuArea();
protected:
    virtual void hoverEnterEvent(QHoverEvent * event)override;
    virtual void hoverMoveEvent(QHoverEvent * event)override;
    virtual void hoverLeaveEvent(QHoverEvent * event)override;

public slots:
};

#endif // CONTROLPANEL_H
