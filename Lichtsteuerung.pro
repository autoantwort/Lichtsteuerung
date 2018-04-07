QT += qml quick

CONFIG += c++14

TARGET = Lichtsteuerung

TEMPLATE = app

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

SOURCES += main.cpp \
    deviceprototype.cpp \
    channel.cpp \
    id.cpp \
    device.cpp \
    programmprototype.cpp \
    programm.cpp \
    applicationdata.cpp \
    namedobject.cpp \
    dmxchannelfilter.cpp \
    syncservice.cpp \
    usermanagment.cpp \
    channelprogrammeditor.cpp \
    modelmanager.cpp \
    mapview.cpp \
    polygon.cpp \
    mapeditor.cpp \
    gridbackground.cpp \
    modelvector.cpp \
    controlpanel.cpp \
    controlitem.cpp \
    controlitemdata.cpp \
    dmxinterface.cpp \
    errornotifier.cpp \
    settings.cpp \
    driver.cpp \
    test/DriverDummy.cpp \
    graph.cpp \
    oscillogram.cpp \
    colorplot.cpp

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
    graph.h \
    oscillogram.h \
    colorplot.h


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

win32-g++{
    #AudioFFT
    LIBS += -L$$PWD/'lib/AudioFFT/dll' -lAudioFFT
    INCLUDEPATH += $$PWD/'lib/AudioFFT/include'
}

win32-msvc{
    #AudioFFT
    LIBS += -L$$PWD/'lib/AudioFFT/dll/AudioFFT.dll'
    INCLUDEPATH += $$PWD/'lib/AudioFFT/include'
}
