#include "compiler.h"
#include "modelmanager.h"
#include <QDebug>
#include <QDir>
#include <QProcess>
#include <QTemporaryFile>
#include <QTextStream>
#include <iostream>
#include <utility>
namespace Modules {

QString Compiler::compilerCmd = QStringLiteral("g++");
QString Compiler::compilerLibraryFlags = QStringLiteral("-shared -fPIC");
QString Compiler::compilerFlags = QStringLiteral("-std=c++17 -g -O3 -Wextra -Wall");
QString Compiler::includePath = QDir::currentPath();

std::pair<int, QString> Compiler::compileAndLoadModule(const QFileInfo &sourceCode, const QString &moduleName, const QString &oldModuleName,
                                                       std::function<std::string(const std::string &)> replaceOldModulesInProgramBlocks) {
    auto moduleFilePath = sourceCode.absolutePath() + "/" + moduleName;
    auto oldModuleFilePath = sourceCode.absolutePath() + "/" + oldModuleName;
    auto result = compileToLibrary(sourceCode, moduleFilePath);
    if (result.first == 0) {
        ModuleManager::singletone()->unloadLibrary(oldModuleFilePath);
        ModuleManager::singletone()->loadModule(ModuleManager::singletone()->getFreeAbsoluteFilePathForModule(moduleFilePath), std::move(replaceOldModulesInProgramBlocks));
    }
    return result;
}

std::pair<int, QString> Compiler::compileToLibrary(const QFileInfo &file, const QString &absoluteNewLibraryFilePath) {

    QString tempOutputFileName = absoluteNewLibraryFilePath + QString::number(rand());
    QProcess p;
#if defined(Q_OS_MAC) || defined(Q_OS_UNIX)
    // p.setEnvironment(QProcess::systemEnvironment()<<"PATH=/usr/bin:/bin:/usr/sbin:/sbin:/usr/local/bin:/Library/TeX/texbin:/usr/local/MacGPG2/bin:/usr/local/share/dotnet:/opt/X11/bin:/Library/Frameworks/Mono.framework/Versions/Current/Commands");
    QString cmd =
        compilerCmd + " " + compilerLibraryFlags + " " + compilerFlags + " " + file.absoluteFilePath() + " -o " + tempOutputFileName + " -I\"/" + QFileInfo(includePath).absoluteFilePath() + "\" ";
    p.start("bash", QStringList() << "-c" << cmd);
#elif defined(Q_OS_WIN)
    tempOutputFileName += QLatin1String(".dll"); // we have to add .dll, otherwise the compiler will add .exe itselves
    QString tempName = tempOutputFileName + ".o";
    QString compilerCMD = compilerCmd.right(compilerCmd.length() - compilerCmd.lastIndexOf('/') - 1);
    QString cmd = /*".\\" + */ compilerCMD + " -c \"" + file.absoluteFilePath() + "\" " + compilerFlags + " -o \"" + tempName + "\" -I\"" + QFileInfo(includePath).absoluteFilePath() + "\" ";
    cmd = "\" " + cmd + " \"";
    qDebug().noquote() << cmd;
    /*
     * Use .\ ans setWorkingDir
     * OR
     * setEnviroment
     */
    // p.setWorkingDirectory(compilerCmd.left(compilerCmd.lastIndexOf('/')));

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert(QStringLiteral("PATH"), env.value(QStringLiteral("Path")) + ";" + compilerCmd.left(compilerCmd.lastIndexOf('/')));
    p.setProcessEnvironment(env);
    // p.setEnvironment(QStringList() <<"PATH"<< compilerCmd.left(compilerCmd.lastIndexOf('/')));
    p.start("cmd /c " + cmd);
#endif
    p.waitForFinished();
    auto err = p.readAllStandardError();
    if (p.exitCode() != 0) {
        qDebug() << p.errorString();
        qDebug() << p.error();
    } else {
#ifdef Q_OS_WIN
        // we only have a .o ans need a .dll
        // cmd: g++ -shared -o foo.dll foo.dll.o
        QProcess oToDll;
        oToDll.setEnvironment(QStringList() << QStringLiteral("PATH") << compilerCmd.left(compilerCmd.lastIndexOf('/')));
        oToDll.start("cmd /c \" " + compilerCMD + " " + compilerLibraryFlags + " -o \"" + tempOutputFileName + "\"  \"" + tempName + "\" \"");
        oToDll.waitForFinished();
        QFile::rename(tempOutputFileName, absoluteNewLibraryFilePath);
        QFile::remove(tempName);
        return {oToDll.exitCode(), err + oToDll.readAllStandardError()};

#else
        if (!QFile::rename(tempOutputFileName, absoluteNewLibraryFilePath)) {
            return {-1, "Can not rename file '" + tempOutputFileName + "' to '" + absoluteNewLibraryFilePath + "'"};
        }
#endif
    }
    return {p.exitCode(), QString(err)};
}

/**
 * @brief Compiler::compileToLibrary writes code into a file ans compile it, @see Compiler::compileToLibrary(QFileInfo,QString)
 * @param code The code to compile to a Module Library
 * @param newLibraryFile The filepath of the new generated library
 * @return return code from the compiler and the output of the compiler
 */
std::pair<int, QString> Compiler::compileToLibrary(const QString &code, const QString &newLibraryFile) {
    QTemporaryFile file;
    file.open();
    {
        QTextStream out(&file);
        out << code;
    }
    file.close();
    return compileToLibrary(file, newLibraryFile);
}

} // namespace Modules
