QT += qml quick

CONFIG += c++14

TARGET = Lichtsteuerung

TEMPLATE = app

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

SOURCES += \
    applicationdata.cpp \
    channel.cpp \
    channelprogrammeditor.cpp \
    colorplot.cpp \
    controlitem.cpp \
    controlitemdata.cpp \
    controlpanel.cpp \
    device.cpp \
    deviceprototype.cpp \
    dmxchannelfilter.cpp \
    dmxinterface.cpp \
    driver.cpp \
    errornotifier.cpp \
    graph.cpp \
    gridbackground.cpp \
    id.cpp \
    main.cpp \
    mapeditor.cpp \
    mapview.cpp \
    modelmanager.cpp \
    modelvector.cpp \
    namedobject.cpp \
    oscillogram.cpp \
    polygon.cpp \
    programm.cpp \
    programmprototype.cpp \
    programms/dmxconsumer.cpp \
    programms/loopprogramm.cpp \
    programms/property.cpp \
    test/testloopprogramm.cpp \
    settings.cpp \
    syncservice.cpp \
    test/DriverDummy.cpp \
    usermanagment.cpp \
    programms/modulemanager.cpp \
    programms/programblock.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += _USE_MATH_DEFINES

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    deviceprototype.h \
    channel.h \
    id.h \
    device.h \
    idbase.h \
    programmprototype.h \
    programm.h \
    applicationdata.h \
    namedobject.h \
    syncservice.h \
    dmxchannelfilter.h \
    usermanagment.h \
    channelprogrammeditor.h \
    modelmanager.h \
    mapview.h \
    polygon.h \
    mapeditor.h \
    gridbackground.h \
    modelvector.h \
    controlpanel.h \
    controlitem.h \
    controlitemdata.h \
    HardwareInterface.h \
    errornotifier.h \
    settings.h \
    driver.h \
    test/DriverDummy.h \
    colorplot.h \
    graph.h \
    oscillogram.h \
    programms/programm.h \
    programms/filter.h \
    programms/types.h \
    programms/property.h \
    programms/loopprogramm.h \
    programms/dmxconsumer.h \
    programms/consumer.h \
    programms/module.h \
    test/testloopprogramm.h \
    programms/modulemanager.h \
    programms/programblock.h


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=


win32-g++{
    LIBS += -L$$PWD/'lib/boost'  -lboost_coroutine -lboost_context
    INCLUDEPATH += $$PWD/'boost'
}

unix{
    #installed with brew install boost
    LIBS += -L/usr/local/Cellar/boost/1.66.0/lib -lboost_coroutine -lboost_context-mt
}
