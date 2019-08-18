#ifndef COMPILER_H
#define COMPILER_H

#include <QFileInfo>
#include <functional>
#include <utility>

namespace Modules {

class Compiler
{
public:
    static QString compilerCmd;
    static QString compilerLibraryFlags;
    static QString compilerFlags;
    static QString includePath;

public:
    static void setCompilerCommand(QString s){compilerCmd = std::move(s);}

    /**
     * @brief Compiler::compileAndLoadModule compiles the code in the file to a module Library and load the module(and unload the old)
     * @note Unloads the old Library with the same name and loads the new Library, replacing all
     *       old Programs/Filters/Consumers in ProgramBlockManager by new in instances
     * @param file a FileInfo Object describing the file where the code is in
     * @param moduleName the name of the Module
     * @param oldModuleName the old name of the Module(needed to unload the old module)
     * @param replaceOldModulesInProgramBlocks if new programs/filters/consumers are loaded old programs/filters/consumers can be replaced, but they can now have another name, so you have to provide a function that maps new names to the old names to replace them. If no function is provided, no modules gets replaced
     * @return return code from the compiler and the output of the compiler
     */
    static std::pair<int,QString> compileAndLoadModule(const QFileInfo &sourceCode, const QString &moduleName, const QString &oldModuleName, std::function<std::string(const std::string&)> replaceOldModulesInProgramBlocks = std::function<std::string(const std::string&)>());
    static std::pair<int,QString> compileToLibrary(const QFileInfo &file, const QString &absoluteNewLibraryFilePath);
    static std::pair<int,QString> compileToLibrary(const QString &code,const QString &newLibraryFile);
};

}  // namespace Modules
#endif // COMPILER_H
