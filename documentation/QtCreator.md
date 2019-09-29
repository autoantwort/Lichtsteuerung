# Qt Creator 
Die IDE mit der die Lichtsteuerung entwickelt wird.

### Einrichten 
1. Mac: open Qt Creator -> About Plugins...  
   Windows: open Help -> About Plugins...
2. Enable:
   - C++ -> ClangCodeModel
   - C++ -> ClangFormat
   - C++ -> ClangRefactoring
   - Code Analyser -> Every Entry
   - Utilities -> Todo  

   Click 'Close' and restart Qt Creator
3. Open the settings (Mac: Qt Creator -> Preferences..., Windows: Tools -> Options...)
4. You can choose under Environment -> Interface -> Theme the 'Dark' Theme ;)
5. Check C++ -> Code Style -> Format instead of indenting  
   Check C++ -> Code Style -> Format while typing  
   Check C++ -> Code Style -> Format edited code on file save
6. Check Qt Quick -> QML/JS Editing -> Enable auto format on file save if [this](https://bugreports.qt.io/browse/QTCREATORBUG-23019) is fixed
7. Close the settings
8. You can select File -> Sessions -> Manage... -> Restore last session on startup

