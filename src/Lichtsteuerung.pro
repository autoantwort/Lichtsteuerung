QT += qml quick networkauth network

CONFIG += c++1z force_debug_info

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
    audio/aubio/aubiocapi.cpp \
    audio/aubio/onsetanalysis.cpp \
    audio/aubio/tempoanalysis.cpp \
    audio/audioeventdata.cpp \
    dmx/channel.cpp \
    gui/audioeventdataview.cpp \
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
    modules/dmxconsumer.cpp \
    modules/ledconsumer.cpp \
    scanner.cpp \
    system_error_handler.cpp \
    test/testloopprogramm.cpp \
    settings.cpp \
    test/DriverDummy.cpp \
    updater.cpp \
    usermanagment.cpp \
    modules/modulemanager.cpp \
    modules/programblock.cpp \
    modules/compiler.cpp \
    test/testmodulsystem.cpp \
    modules/controller.cpp \
    modules/dmxprogram.cpp \
    modules/dmxprogramblock.cpp \
    modules/json_storage.cpp \
    codeeditorhelper.cpp \
    test/testmodulecontroller.cpp \
    test/testprogrammblock.cpp \
    gui/programblockeditor.cpp \
    audio/sample.cpp \
    test/testsampleclass.cpp \
    audio/audiocapturemanager.cpp \
    modules/boostloopprogramcontextswitcher.cpp \
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
    sortedmodelview.cpp \
    zip.cpp

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
    audio/aubio/aubiocapi.h \
    audio/aubio/onsetanalysis.h \
    audio/aubio/tempoanalysis.h \
    audio/audioeventdata.h \
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
    modules/controlpoint.hpp \
    modules/ledconsumer.h \
    modules/scanner.hpp \
    scanner.h \
    system_error_handler.h \
    updater.h \
    usermanagment.h \
    gui/audioeventdataview.h \
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
    modules/types.h \
    modules/dmxconsumer.h \
    modules/module.h \
    test/testloopprogramm.h \
    modules/modulemanager.h \
    modules/programblock.h \
    modules/property.hpp \
    modules/filter.hpp \
    modules/consumer.hpp \
    modules/compiler.h \
    test/testmodulsystem.h \
    modules/controller.h \
    modules/dmxprogram.h \
    modules/dmxprogramblock.h \
    modules/storage.hpp \
    modules/json_storage.h \
    codeeditorhelper.h \
    modules/program.hpp \
    test/testmodulecontroller.h \
    test/testprogrammblock.h \
    gui/programblockeditor.h \
    audio/sample.h \
    test/testsampleclass.h \
    audio/audiocapturemanager.h \
    modules/fftoutput.hpp \
    modules/loopprogram.hpp \
    modules/boostloopprogramcontextswitcher.h \
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
    modules/spotifyobjetcs.hpp \
    modules/spotify.hpp \
    sortedmodelview.h \
    zip.h


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

win32-g++{
    # boost
    CONFIG(debug, debug|release){
        DEBUG = d-
    } else {
        DEBUG =
    }
    LIBS += -L$$PWD/'lib/boost/lib' -lboost_coroutine-mt-$${DEBUG}x64 -lboost_context-mt-$${DEBUG}x64
    INCLUDEPATH += $$PWD/'lib/boost/include'
}

unix{
    #installed with brew install boost
    LIBS += -L/usr/local/lib -lboost_coroutine -lboost_context-mt
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

win32-g++{
    #DrMinGW
    #LIBS += -L$$PWD/'lib/AudioFFT/dll' -lAudioFFT
    !contains(QT_ARCH, i386){ # 64 bit
        LIBS += -L$$PWD/'lib/DrMinGW/bin/' -lexchndl
        INCLUDEPATH += $$PWD/'lib/DrMinGW/include'
        DEFINES += DrMinGW
    }
}

win32-g++{
    #Aubio
    !contains(QT_ARCH, i386){ # 64 bit
        LIBS += -L$$PWD/'lib/aubio/lib/' -laubio-5
        INCLUDEPATH += $$PWD/'lib/aubio/include'
    }
}

    #segvcatch
    LIBS += -L$$PWD/'lib/segvcatch/lib' -lsegvcatch
    INCLUDEPATH += $$PWD/'lib/segvcatch/include'


    #boost stacktrace / libbacktrace
    #see https://www.boost.org/doc/libs/1_66_0/doc/html/stacktrace/configuration_and_build.html
    DEFINES += BOOST_STACKTRACE_USE_BACKTRACE
    LIBS += -L$$PWD/'lib/libbacktrace/lib' -lbacktrace
    INCLUDEPATH += $$PWD/'lib/libbacktrace/include'

win32-msvc{
    #AudioFFT
    LIBS += -L$$PWD/'lib/AudioFFT/dll/AudioFFT.dll'
    INCLUDEPATH += $$PWD/'lib/AudioFFT/include'
}
