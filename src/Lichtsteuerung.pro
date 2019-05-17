QT += qml quick networkauth

CONFIG += c++1z

TARGET = Lichtsteuerung

TEMPLATE = app

RESOURCES += qml.qrc

DEFINES += CONVERT_FROM_SPOTIFY_OBJECTS

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

SOURCES += \
    applicationdata.cpp \
    dmx/channel.cpp \
    gui/channelprogrammeditor.cpp \
    gui/colorplot.cpp \
    gui/controlitem.cpp \
    gui/controlitemdata.cpp \
    gui/controlpanel.cpp \
    dmx/device.cpp \
    dmx/deviceprototype.cpp \
    dmx/dmxchannelfilter.cpp \
    dmx/driver.cpp \
    errornotifier.cpp \
    gui/graph.cpp \
    gui/gridbackground.cpp \
    id.cpp \
    main.cpp \
    gui/mapeditor.cpp \
    gui/mapview.cpp \
    modelmanager.cpp \
    modelvector.cpp \
    dmx/namedobject.cpp \
    gui/oscillogram.cpp \
    gui/polygon.cpp \
    dmx/programm.cpp \
    dmx/programmprototype.cpp \
    programms/dmxconsumer.cpp \
    scanner.cpp \
    test/testloopprogramm.cpp \
    settings.cpp \
    test/DriverDummy.cpp \
    usermanagment.cpp \
    programms/modulemanager.cpp \
    programms/programblock.cpp \
    programms/compiler.cpp \
    test/testmodulsystem.cpp \
    programms/controller.cpp \
    programms/dmxprogram.cpp \
    programms/dmxprogramblock.cpp \
    programms/json_storage.cpp \
    codeeditorhelper.cpp \
    test/testmodulecontroller.cpp \
    test/testprogrammblock.cpp \
    gui/programblockeditor.cpp \
    audio/sample.cpp \
    test/testsampleclass.cpp \
    audio/audiocapturemanager.cpp \
    programms/boostloopprogramcontextswitcher.cpp \
    spotify/spotify.cpp \
    spotify/currentplayingobject.cpp \
    spotify/trackobject.cpp \
    spotify/albumobject.cpp \
    spotify/artistobject.cpp \
    spotify/imageobject.cpp \
    spotify/audiofeaturesobject.cpp \
    spotify/timeintervalobject.cpp \
    spotify/sectionobject.cpp \
    spotify/segmentobject.cpp \
    spotify/audioanalysisobject.cpp \
    spotify/userobject.cpp \
    sortedmodelview.cpp

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
    dmx/deviceprototype.h \
    dmx/channel.h \
    id.h \
    dmx/device.h \
    idbase.h \
    dmx/programmprototype.h \
    dmx/programm.h \
    applicationdata.h \
    dmx/namedobject.h \
    dmx/dmxchannelfilter.h \
    programms/controlpoint.hpp \
    programms/scanner.hpp \
    scanner.h \
    usermanagment.h \
    gui/channelprogrammeditor.h \
    modelmanager.h \
    gui/mapview.h \
    gui/polygon.h \
    gui/mapeditor.h \
    gui/gridbackground.h \
    modelvector.h \
    gui/controlpanel.h \
    gui/controlitem.h \
    gui/controlitemdata.h \
    dmx/HardwareInterface.h \
    errornotifier.h \
    settings.h \
    dmx/driver.h \
    test/DriverDummy.h \
    gui/colorplot.h \
    gui/graph.h \
    gui/oscillogram.h \
    programms/types.h \
    programms/dmxconsumer.h \
    programms/module.h \
    test/testloopprogramm.h \
    programms/modulemanager.h \
    programms/programblock.h \
    programms/property.hpp \
    programms/filter.hpp \
    programms/consumer.hpp \
    programms/compiler.h \
    test/testmodulsystem.h \
    programms/controller.h \
    programms/dmxprogram.h \
    programms/dmxprogramblock.h \
    programms/storage.hpp \
    programms/json_storage.h \
    codeeditorhelper.h \
    programms/program.hpp \
    test/testmodulecontroller.h \
    test/testprogrammblock.h \
    gui/programblockeditor.h \
    audio/sample.h \
    test/testsampleclass.h \
    audio/audiocapturemanager.h \
    programms/fftoutput.hpp \
    programms/loopprogram.hpp \
    programms/boostloopprogramcontextswitcher.h \
    spotify/spotify.h \
    spotify/currentplayingobject.h \
    spotify/trackobject.h \
    spotify/albumobject.h \
    spotify/artistobject.h \
    spotify/imageobject.h \
    spotify/audiofeaturesobject.h \
    spotify/timeintervalobject.h \
    spotify/sectionobject.h \
    spotify/segmentobject.h \
    spotify/audioanalysisobject.h \
    spotify/userobject.h \
    spotify/util.h \
    programms/spotifyobjetcs.hpp \
    programms/spotify.hpp \
    sortedmodelview.h


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

# QMAKE_CXXFLAGS += -fsanitize=address
# QMAKE_CXXFLAGS += -fno-omit-frame-pointer
# QMAKE_CXXFLAGS += -Wshadow
# QMAKE_CXXFLAGS += -lasan
# LIBS += -lasan

INCLUDEPATH += $$PWD/../

win32-msvc{
    LIBS += -L$$PWD/lib/boost/
}

win32-g++{
    CONFIG(debug, debug|release){
        DEBUG = d-
    } else {
        DEBUG =
    }
    contains(QT_ARCH, i386){ # 32 bit
        BITS = 32
        LIBS += -L$$PWD/'lib/boost'  -lboost_coroutine -lboost_context
    } else { # 64 bit
        BITS = 64
        # use the mingw73 builds
        LIBS += -L$$PWD/'lib/boost/mingw73'  -lboost_coroutine-mgw73-mt-$${DEBUG}x64-1_69 -lboost_context-mgw73-mt-$${DEBUG}x64-1_69
    }
    INCLUDEPATH += $$PWD/'boost'
}

unix{
    #installed with brew install boost
    LIBS += -L/usr/local/lib -lboost_coroutine -lboost_context-mt
}
win32-msvc{
    LIBS += -L$$PWD/lib/boost/
}

macx{
    #AudioFFT
    LIBS += -L$$PWD/'lib/AudioFFT/dll' -lAudioFFT.1
    INCLUDEPATH += $$PWD/'lib/AudioFFT/include'
    #https://forum.qt.io/topic/59209/solved-osx-deployment-fatal-error-with-dylib-library-not-loaded-image-not-found/4
    Resources.files += libAudioFFT.1.dylib
    Resources.path = Contents/MacOS
    QMAKE_BUNDLE_DATA += Resources
}

win32-g++{
    #AudioFFT
    #LIBS += -L$$PWD/'lib/AudioFFT/dll' -lAudioFFT
    contains(QT_ARCH, i386){ # 32 bit
        LIBS += -L$$PWD/'lib/AudioFFT/dll/win32/' -lAudioFFT
    } else { # 64 bit
        LIBS += -L$$PWD/'lib/AudioFFT/dll/win64' -lAudioFFT
    }
    INCLUDEPATH += $$PWD/'lib/AudioFFT/include'
}

win32-msvc{
    #AudioFFT
    LIBS += -L$$PWD/'lib/AudioFFT/dll/AudioFFT.dll'
    INCLUDEPATH += $$PWD/'lib/AudioFFT/include'
}
