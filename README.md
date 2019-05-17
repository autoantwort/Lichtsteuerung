# Lichtsteuerung 
Die neue Lichtsteuerung für die Bar in C++/[Qt](https://de.wikipedia.org/wiki/Qt_(Bibliothek)).  
Features:
 * Einfacherere Erstellung von Programmen für DMX Geräte
 * Anpassbares Control Pane
 * Karte der Bar um Lampen auswählen und steuern zu können
 * Jede Lampe gezielt steuerbar
 * Erweiterbar durch Plugins (Modules)
 * Capturing des Windows Outputs zur Weiterverarbeitung 
 * Anmeldung mittels Spotify um auf Informationen vom aktuell gespielten Lied zuzugreifen 
 * Platformunabhängig 

## Installation (zum Entwickeln)

1. Lade [Qt](https://www.qt.io/download-qt-installer) herunter
2. Starte den Installer. 
3. Wenn du dich anmelden sollst, drücke unten rechts auf _Skip_
4. Unter _Latest releases_ wähle die neuste Version aus und installiere die Punkte:
     * macOS (unter Mac)
     * MinGW 7.3.0 64-Bit (unter Windows)
     * Desktop gcc 64-bit (unter Linux)
     * Qt Network Authorization (egal welches OS)
5. Drücke _Next_ und beende die Installation. 

## Abhängigkeiten 
Die Lichtsteuerung hängt von ein paar anderen Projekten ab. 
#### [Boost](https://www.boost.org/)
Boost wird benötigt, um bei Modules nicht lineare Codeausführung zu ermöglichen (Corotines). Boost läst sich unter Linux und Windows durch einen beliebigen Paketmanager installieren, für Windows ist die Anleitung [hier](boost.md) zu finden.  
Hinweis für Windows: Im git liegen schon alle benötigten Header und libs, man muss boost nicht neu kompilieren. Man muss dies nur machen um boost zu updaten (dann bitte neue libs und header ins git pushen).

#### [Capture_Windows_SoundOutput](https://git.rwth-aachen.de/leander.schulten/Capture_Windows_SoundOutput)
Dieses Project ermöglicht das abfangen des Windows Audioausgangs. Für andere Plattformen ligen keine implementierungen vor, diese sind aber herzlich willkommen. Die kompilierte Version ist hier im git vorhanden.

#### [AudioFFT](https://git.rwth-aachen.de/leander.schulten/FFT_for_Audio)
Dieses Projekt ermöglicht es, den abgefangenen Sound auf Frequenzbereiche aufzuspalten, wobei jedem Frequenzbereich eine "Energie"/Stärke zugeordnet wird. Dieses Projekt hängt wiederum von [fftw](http://www.fftw.org/) ab. Diese Abhängigkeiten befinden sich auch hier als kompilierte lib im git. 

#### [SusbDmx-Driver](https://git.rwth-aachen.de/leander.schulten/SusbDMX-Driver)
Dieses Projekt enthält den aktuellen Treiber, der es ermöglicht, den USB zu DMX Konverter anzusteuern. Dieser Treiber befindet sich nicht als lib hier im git. 

##### [LedConsumer](https://git.rwth-aachen.de/leander.schulten/comconsumer)
Ein Consumer für die LEDs für das Module System. Nicht hier im git enthalten.

## Struktur des Projekts 
Das Projekt ist aktuell auf Ordnerebene nicht in einzelne Bereiche getrennt, jedoch kann man folgende Bereiche feststellen:

###### DMX
Hier fallen alle Klassen rein, die sich um DMX kümmern. Die Verwaltung der Geräte und Programme sieht folgendermaßen aus:  
Es existieren DevicePrototypen, sowas kann z.B. eine Lampe, eine Scanner Art oder ein Stroboskop sein. Diese werden durch einen Namen und durch eine Anzahl an Channels definiert, wobei jeder Channel einen Namen besitzt.  
Ein Device ist ein real existierendes Gerät (z.B. die Lampe an der Wand über dem Kicker links), es setzt sich zusammen aus einem Namen, einem DevicePrototyp, einem startDMXChannel und einer Position im Raum.  
Ein ProgrammPrototyp ein ist Programm für ein bestimmten DevicePrototype, es besteht aus Programmen für jeden Channel eines Programmprototypen. So ein Channelprogramm ist definiert durch (Time,Value) Paare. 
Ein Programm ist eine Sammlung von (Device,ProgrammPrototyp) Paaren(DeviceProgramm), sodass jedem Device in dem Programm ein Programmprototyp zugeordnet ist, welches definiert welcher Channel des Devices zum Zeitpunkt t welchen Wert haben soll. Jedes DeviceProgramm besitzt ein offset, um welchen die Zeit t für dieses DeviceProgramm verschoben wird. Das Programm selbst verfügt noch über eine Geschwindigkeit. 
Jeder DMXChannel, unabhängig ob ein Gerät den Channel benutzt oder nicht, besitzt einen DMXChannelFilter. Dieser legt für seinen Channel einen default Value fest. Außerdem kann er das Ergebnis eines Programms filtern, indem er die den Wertebereich, den den Channel annehmen kann, verändert. Weiterhin kann der ChannelFilter die Ausgabe eines Programms ignorieren und überschreiben. 
Die bestimmung eines Wertes eines DMXChannels sieht als folgendermaßen aus:  
 1. DeviceProgramm setzt default Wert 
 2. Optional: Programm schreibt Wert in Channel (wenn für den Channel/das Device eins läuft)
 3. Optional: der DMXConsumer des Modules-Systems (später mehr) schreibt einen Wert in den Channel
 4. Channelfiler filtert den aktuellen Wert
 5. Optional: Channelfilter überschreibt ggf. den aktuellen Wert 

###### GUI Elemente
Es existieren Klassen, die GUI Elemente repräsentiern, wie z.B. der ChannelProgrammeditor oder der MapView

###### Modules 
Das Modules System befindet sich im Ordner programms/ und ist folgendermaßen Aufgebaut: 
Es gibt Programme, Filter und Consumer. Programme haben einen Ausgang, der einen bestimmten Typ und eine dynamische Länge hat(änderbar, aber nicht während das Programm läuft). Dass Programm wird von Controller alle x Millisekunden aufgerufen und kann dann Werte in den Output schreiben. Ein Filter hat einen Eingang eines bestimmten Typs und eine dynamische Länge sowie einen Ausgang eines bestimmten Typs und eine dynamische Länge (Ein- und Ausgang Typ und Länge können unterschiedlich sein). Wenn sich die Eingangsdaten des Filters geändert haben wird die `filter` Funktion des Filters aufgerufen und der Filter kann die gefilterten Werte in den Ausgang schreiben. Weiterhin wird der Filter alle x Sekunden vom Controller aufgerufen, um Effekte wie z.B. Fadeouts zu realisieren. Consumer haben einen Eingang eines bestimmten Typs und eine dynamische Länge und sind dafür da, um die Werte des Eingangs an ein bestimmtes Ausgabegerät zu senden, z.B. an LEDS oder DMX.  
Ein ProgrammBlock beschreibt, welche Programme wie mit welchen Filtern und wie welche Filter mit Consumern verbunden sind, bzw. Programme direkt mit Consumern. Jedes Programm/Filter/Consumer kann verschiedene Eigenschaften haben die in der GUI vom Benutzer verändert werden können (z.B. Geschwindigkeit eines Programms). Programme und Filter können in der GUI geschreiben werden. Der CodeEditorHelper generiert daraus Code der Kompiliert werden und vom ModuleManager geladen werden kann.  
In Programmen und Filtern kann auf die FFT Analyse des Audioausgangs zugegriffen werden sowie auf Daten von Spotify wie aktuelles Lied und die Audio Analyse von Spotify zum aktuellen Lied. 

## GUI
Die Oberfläche ist in [QML](https://de.wikipedia.org/wiki/QML) geschrieben, wobei die Oberflächen Elemente aus [Qt Quick Controls 2](https://doc.qt.io/qt-5/qtquick-controls2-qmlmodule.html) stammen. Damit C++ Objekte in QML, bzw. Javascript verwendet werden können, müssen diese von [QObject](https://doc.qt.io/qt-5/qobject.html#details)(lesen) erben. Damit Eigenschaften in QML sichtbar sind werden diese mittels [Q_PROPERTY](https://doc.qt.io/qt-5/properties.html) Qt bekannt gemacht. Ein schönes Feature von QML sind die [Property Bindings](https://doc.qt.io/qt-5/qtqml-syntax-propertybinding.html)(lesen). Damit diese Funktionieren ist im Hintergrund das [Signal and Slot System](https://doc.qt.io/qt-5/signalsandslots.html) zuständig. Mittels `engine.rootContext()->setContextProperty("test",testObject);` ist es möglich, C++/QObject Objekte in QML/Javascript verfügbar zu machen, sodass von überall auf diese Zugegriffen werden kann. In diese Projekt wird das vielfach von [Models](https://doc.qt.io/qt-5/qabstractitemmodel.html#details) genutzt. Die Modelle werden in C++ gefüllt und verwaltet und in QML für [ListViews](https://doc.qt.io/qt-5/qml-qtquick-listview.html) als Model benutzt. Zum weiteren lernen ist es Hilfreich, sich einfach QML Code anzuschauen und die Dokumentation der einzelnen Komponenten zu lesen. 


