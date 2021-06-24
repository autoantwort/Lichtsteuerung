
#set(QAPPLICATION_CLASS QGuiApplication CACHE STRING "Inheritance class for SingleApplication")
#add_subdirectory(lib/SingleApplication/git)

# QtQuick compiler. See https://doc.qt.io/QtQuickCompiler/qquickcompiler-building-with-cmake.html
find_package(Qt5QuickCompiler)
qtquick_compiler_add_resources(RESOURCES qml.qrc)

add_executable(lichtsteuerung
    applicationdata.cpp applicationdata.h
    audio/aubio/aubiocapi.cpp audio/aubio/aubiocapi.h
    audio/aubio/onsetanalysis.cpp audio/aubio/onsetanalysis.h
    audio/aubio/spectrumanalysis.cpp audio/aubio/spectrumanalysis.h
    audio/aubio/tempoanalysis.cpp audio/aubio/tempoanalysis.h
    audio/audiocapturemanager.cpp audio/audiocapturemanager.h
    audio/audioeventdata.cpp audio/audioeventdata.h
    audio/remotevolume.cpp audio/remotevolume.h
    audio/sample.cpp audio/sample.h
    audio/systemvolume.cpp audio/systemvolume.h
    codeeditorhelper.cpp codeeditorhelper.h
    dmx/HardwareInterface.h
    dmx/channel.cpp dmx/channel.h
    dmx/device.cpp dmx/device.h
    dmx/deviceprototype.cpp dmx/deviceprototype.h
    dmx/dmxchannelfilter.cpp dmx/dmxchannelfilter.h
    dmx/driver.cpp dmx/driver.h
    dmx/namedobject.cpp dmx/namedobject.h
    dmx/programm.cpp dmx/programm.h
    dmx/programmprototype.cpp dmx/programmprototype.h
    errornotifier.cpp errornotifier.h
    gui/audioeventdataview.cpp gui/audioeventdataview.h
    gui/channelprogrammeditor.cpp gui/channelprogrammeditor.h
    gui/colorplot.cpp gui/colorplot.h
    gui/controlitem.cpp gui/controlitem.h
    gui/controlitemdata.cpp gui/controlitemdata.h
    gui/controlpanel.cpp gui/controlpanel.h
    gui/graph.cpp gui/graph.h
    gui/gridbackground.cpp gui/gridbackground.h
    gui/linegeometry.cpp gui/linegeometry.h
    gui/mapeditor.cpp gui/mapeditor.h
    gui/mapview.cpp gui/mapview.h
    gui/oscillogram.cpp gui/oscillogram.h
    gui/polygon.cpp gui/polygon.h
    gui/programblockeditor.cpp gui/programblockeditor.h
    id.cpp id.h
    idbase.h
    main.cpp
    modelmanager.cpp modelmanager.h
    modelvector.cpp modelvector.h
    modules/boostloopprogramcontextswitcher.cpp modules/boostloopprogramcontextswitcher.h
    modules/compiler.cpp modules/compiler.h
    modules/consumer.hpp
    modules/controller.cpp modules/controller.h
    modules/controlpoint.hpp
    modules/dmxconsumer.cpp modules/dmxconsumer.h
    modules/dmxprogram.cpp modules/dmxprogram.h
    modules/dmxprogramblock.cpp modules/dmxprogramblock.h
    modules/fftoutput.hpp
    modules/filter.hpp
    modules/json_storage.cpp modules/json_storage.h
    modules/ledconsumer.cpp modules/ledconsumer.h
    modules/loopprogram.hpp
    modules/module.h
    modules/modulemanager.cpp modules/modulemanager.h
    modules/mqtt.hpp
    modules/mqttimpl.cpp modules/mqttimpl.h
    modules/program.hpp
    modules/programblock.cpp modules/programblock.h
    modules/property.hpp
    modules/scanner.hpp
    modules/spotify.hpp
    modules/spotifyobjetcs.hpp
    modules/storage.hpp
    modules/types.h
    scanner.cpp scanner.h
    settings.cpp settings.h
    settingsfilewrapper.cpp settingsfilewrapper.h
    slideshow.cpp slideshow.h
    sortedmodelview.cpp sortedmodelview.h
    span.h
    spotify/albumobject.cpp spotify/albumobject.h
    spotify/artistobject.cpp spotify/artistobject.h
    spotify/audioanalysisobject.cpp spotify/audioanalysisobject.h
    spotify/audiofeaturesobject.cpp spotify/audiofeaturesobject.h
    spotify/currentplayingobject.cpp spotify/currentplayingobject.h
    spotify/imageobject.cpp spotify/imageobject.h
    spotify/sectionobject.cpp spotify/sectionobject.h
    spotify/segmentobject.cpp spotify/segmentobject.h
    spotify/spotify.cpp spotify/spotify.h
    spotify/timeintervalobject.cpp spotify/timeintervalobject.h
    spotify/trackobject.cpp spotify/trackobject.h
    spotify/userobject.cpp spotify/userobject.h
    spotify/util.h
    system_error_handler.cpp system_error_handler.h
    test/DriverDummy.cpp test/DriverDummy.h
    test/testloopprogramm.cpp test/testloopprogramm.h
    test/testmodulecontroller.cpp test/testmodulecontroller.h
    test/testmodulsystem.cpp test/testmodulsystem.h
    test/testprogrammblock.cpp test/testprogrammblock.h
    test/testsampleclass.cpp test/testsampleclass.h
    updater.cpp updater.h
    usermanagment.cpp usermanagment.h
    ${RESOURCES}
)

target_compile_definitions(lichtsteuerung PRIVATE
#    BOOST_STACKTRACE_USE_BACKTRACE
    CONVERT_FROM_SPOTIFY_OBJECTS
    QT_DEPRECATED_WARNINGS
    QT_MESSAGELOGCONTEXT
    _USE_MATH_DEFINES
)
target_link_libraries(lichtsteuerung Qt5::Qml Qt5::Quick Qt5::Network Qt5::Widgets Qt5::NetworkAuth Qt5::WebSockets)

# Aubio
find_path(AUBIO_INCLUDE_DIR aubio/aubio.h)
find_library(AUBIO_LIBRARY aubio)
target_include_directories(lichtsteuerung PRIVATE ${AUBIO_INCLUDE_DIR})
target_link_libraries(lichtsteuerung ${AUBIO_LIBRARY})

# RtAudio
find_path(RtAudio_INCLUDE_DIR rtaudio/RtAudio.h)
target_include_directories(lichtsteuerung PRIVATE ${RtAudio_INCLUDE_DIR})
find_library(RtAudio_LIBRARY rtaudio)
target_link_libraries(lichtsteuerung ${RtAudio_LIBRARY})
if(APPLE)
    target_link_libraries(lichtsteuerung "-framework CoreAudio")
    target_link_libraries(lichtsteuerung "-framework CoreFoundation")
elseif(WIN32)
    target_link_libraries(lichtsteuerung ole32 winmm ksuser mfplat mfuuid wmcodecdspuuid)
endif()

# boost
find_package(Boost 1.75.0 REQUIRED COMPONENTS thread context coroutine)
target_link_libraries(lichtsteuerung ${Boost_LIBRARIES})
target_include_directories(lichtsteuerung PRIVATE ${Boost_INCLUDE_DIRS})

if(APPLE)
    # damit boost stacktrace funktioniert
    target_compile_definitions(lichtsteuerung PRIVATE _GNU_SOURCE)
endif()



if(WIN32)
    # DrMinGW
    target_compile_definitions(lichtsteuerung PRIVATE DrMinGW)
    target_include_directories(lichtsteuerung PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/DrMinGW/include)
    target_link_directories(lichtsteuerung PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/DrMinGW/bin)
    target_link_libraries(lichtsteuerung exchndl)
endif()

# Needed by the SystemVolume class
if(APPLE)
    target_link_libraries(lichtsteuerung "-framework CoreAudio")
elseif(CMAKE_SYSTEM_NAME MATCHES Linux)
    # needed for dynamic libs
    target_link_libraries(lichtsteuerung dl)
    # needed for the SystemVolume class
    target_link_libraries(lichtsteuerung asound)
endif()

# SpanLite
find_path(SpanLite_INCLUDE_DIR nonstd/span.hpp)
target_include_directories(lichtsteuerung PRIVATE ${SpanLite_INCLUDE_DIR})
target_compile_definitions(lichtsteuerung PRIVATE USE_SPAN_LITE)

# SingleApplication
# target_link_libraries(${PROJECT_NAME} SingleApplication::SingleApplication)

# kubazip
find_package(kubazip REQUIRED)
target_link_libraries(lichtsteuerung kubazip::kubazip)

# Qt MQTT
target_include_directories(lichtsteuerung PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/qtmqtt/include)
if(APPLE)
    set_target_properties(lichtsteuerung PROPERTIES LINK_FLAGS "-Wl,-F${CMAKE_CURRENT_SOURCE_DIR}/lib/qtmqtt/lib")
    target_link_libraries(lichtsteuerung "-framework QtMqtt")
else()
    target_link_directories(lichtsteuerung PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/qtmqtt/lib)
    target_link_libraries(lichtsteuerung Qt5Mqtt)
endif()

# Segvcatch
target_include_directories(lichtsteuerung PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/segvcatch/include)
target_link_directories(lichtsteuerung PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/segvcatch/lib)
target_link_libraries(lichtsteuerung segvcatch)
