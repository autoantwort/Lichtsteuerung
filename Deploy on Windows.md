Deploy on Windows
=================

1. Build for release in Qt Creator
2. Goto the release folder
3. Delete everything except the Lichtsteuerung.exe file
4. Open the command line and run `C:\Qt\<your qt version>\mingw53_32\bin\windeployqt.exe --qmldir C:\Qt\<your qt version>\mingw53_32\qml <path to the Lichtsteuerung file>`
5. Copy the driver and the the properties json file to the release folder
6. Maybe copy `libgcc_s_dw2-1.dll`, `libstdc++-6.dll` and `libwinpthread-1.dll` from C:\Qt\<your qt version>\mingw53_32\bin\ to the release folder
7. Copy the C:\Qt\<your qt version>\mingw53_32\qml\QtQuick\Shapes folder into the QtQuick folder of the release folder 
8. Finished, you can now distribute the application