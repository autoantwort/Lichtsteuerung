#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QQuickItem>
#include "programm.h"
#include "programms/programblock.h"
#include "controlitemdata.h"

namespace GUI{

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
    QQmlComponent programBlock;
    int menuHeight;
    int menuWidth;
protected:
    /**
     * @brief createControlItem creates an ControlItem defined by component and set the ControlItemData field of the ControlData to data
     * @param component The component that should be defined, must be a subclass of "qrc:/ControlPane/ControlItem.qml"
     * @param data The DataObject that holds the information, this object is owned by the instance created from the component
     */
    void createControlItem(QQmlComponent & component, ControlItemData * data);
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
    /**
     * @brief addProgramBlockControl adds a ProgramBlockControlItem to the panel to start and stop Programblocks
     * @param programBlock the ProgramBlock that the controlItem should control
     */
    Q_INVOKABLE void addProgramBlockControl(Modules::ProgramBlock * programBlock);
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

} // namespace GUI

#endif // CONTROLPANEL_H
