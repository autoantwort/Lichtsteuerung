#ifndef COMPILER_H
#define COMPILER_H
#include <QFile>

namespace Modules {

class Compiler
{
    static QString compilerCmd;
    static QString compilerLibraryFlags;
    static QString compilerFlags;

public:
    Compiler();
    static void setCompilerCommand(QString s){compilerCmd = s;}
    static int compileToLibrary(const QFile &file,const QString &newLibraryFile);
    static int compileToLibrary(const QString &code,const QString &newLibraryFile);
};


}
#endif // COMPILER_H
