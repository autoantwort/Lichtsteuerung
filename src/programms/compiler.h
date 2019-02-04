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

    /**
     * @brief Compiler::compileAndLoadModule compiles the code in the file to a module Library and load the module(and unload the old)
     * @note Unloads the old Library with the same name and loads the new Library, replacing all
     *       old Programs/Filters/Consumers in ProgramBlockManager by new in instances
     * @param file a FileInfo Object describing the file where the code is in
     * @param moduleName the name of the Module
     * @return return code from the compiler and the output of the compiler
     */
    static std::pair<int,QString> compileAndLoadModule(const QFileInfo &sourceCode, const QString &moduleName);
    static std::pair<int,QString> compileToLibrary(const QFileInfo &file, const QString &absoluteNewLibraryFilePath);
    static std::pair<int,QString> compileToLibrary(const QString &code,const QString &newLibraryFile);
};


}
#endif // COMPILER_H
