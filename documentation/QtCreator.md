# Qt Creator 
Die IDE mit der die Lichtsteuerung entwickelt wird.

### Einrichten 
1. Mac: open Qt Creator -> About Plugins...  
   Windows/Linux: open Help -> About Plugins...
2. Enable:
   - C++ -> ClangCodeModel
   - C++ -> ClangFormat
   - C++ -> ClangRefactoring
   - Code Analyser -> Every Entry
   - Utilities -> Todo  

   Click 'Close' and restart Qt Creator
3. Open the settings (Mac: Qt Creator -> Preferences..., Windows/Linux: Tools -> Options...)
4. You can choose under Environment -> Interface -> Theme the 'Dark' Theme ;)
5. Check C++ -> Code Style -> Format instead of indenting  
   Check C++ -> Code Style -> Format while typing  
   Check C++ -> Code Style -> Format edited code on file save
6. Check Qt Quick -> QML/JS Editing -> Enable auto format on file save, **if [this](https://bugreports.qt.io/browse/QTCREATORBUG-23019) is fixed**
7. Close the settings
8. You can select File -> Sessions -> Manage... -> Restore last session on startup
9. If you have changed the theme, restart Qt Creator

### Compiler
Unter **Windows** wird der Compiler von Qt mitgeliefert.  
Unter **Linux** kann gcc mittels `sudo apt install gcc` installiert werden. Gcc sollte mindestens version 7 haben (überprüfen mit `gcc --version`).  
Unter **Mac**: XCode im AppStore installieren. 

### Cpp-Check installieren
[Cppcheck](http://cppcheck.sourceforge.net/) ist ein Tool zur statischen Analyse, um Fehler zu finden.
* Windows: Auf die Webseite von [Cppcheck](http://cppcheck.sourceforge.net/) gehen, den Installer runterladen und installieren. 
* Mac: `brew install cppcheck` (vorrausgesetzt [brew](https://brew.sh/) ist installiert)
* Linux: `sudo apt install cppcheck`