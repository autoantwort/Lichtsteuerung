#ifndef COMPILER_H
#define COMPILER_H
#include <QFileInfo>

namespace Modules {

class Compiler
{
public:
    static QString compilerCmd;
    static QString compilerLibraryFlags;
    static QString compilerFlags;
    static QString includePath;

public:
    Compiler();
    static void setCompilerCommand(QString s){compilerCmd = s;}
    static std::pair<int,QString> compileToLibrary(const QFileInfo &file,const QString &newLibraryFile);
    static std::pair<int,QString> compileToLibrary(const QString &code,const QString &newLibraryFile);
};


}
#endif // COMPILER_H
