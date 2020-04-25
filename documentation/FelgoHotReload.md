# Felgo Hot Reload
Only works on Linux and Mac, not on Windows (because Felgo distributes only 32 Bit binaries, but all dependencies are in 64 Bit)

## Installation and Use
1. Visit the [Felgo Download Page](https://felgo.com/download) and click _Download_. You have to create an account if you don't have one :(
2. Install Felgo
3. Open Felgo and login with your account
4. Multiple Windows will open. Go to the main window (QtCreator) and open the Lichtsteuerung Project (If you have enabled _File_ -> _Sessions_ -> _Restore last session on startup_ the project is maybe already there).
5. Go to the _Projects_ tab, select the _Lichtsteuerung_ project in the upper left corner. A _Configure Project_ page should now appear. Select the _Felgo Desktop_ item and click _Configure Project_. 
6. Open the _Lichtsteuerung.pro_ file in the Qt Creator, uncomment the line `DEFINES += WITH_FELGO` (remove the `#` at the beginning of the line) and save the file.
7. Switch to the _FelgoLiveServer_ Window (if there is no _FelgoLiveServer_ window, hit the _Live_ button on the left side of the Qt Creator Window). If a window with the name _FelgoLiveClient_ opens or exists, close that window. 
8. Disable the option _Auto Start_ in the _FelgoLiveServer_ Window
9. Now click the _Run_ button in the Qt Creator window (the _Lichtsteuerung_ target should be selected in the display symbol over the _Run_ button). The _Lichtsteuerung_ should now gets builded and start with hot reloading (you first see a window with a connect button, do nothing until you see the Lichtsteuerung)
10. Now you can edit the QML files. If you save a file, the UI gets automatically updated (it can took up to 10 seconds if the Lichtsteuerungs window is not in focus)
