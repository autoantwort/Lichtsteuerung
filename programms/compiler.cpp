#include "compiler.h"
#include <QTemporaryFile>
#include <QTextStream>
#include <QProcess>
#include <QDebug>
#include <iostream>
#include <QDir>
#include "modelmanager.h"
namespace Modules{



QString Compiler::compilerCmd = "g++";
QString Compiler::compilerLibraryFlags = "-shared -fPIC";
QString Compiler::compilerFlags = "-std=c++14 -O3";
QString Compiler::includePath = QDir::currentPath();


Compiler::Compiler()
{

}


std::pair<int,QString> Compiler::compileAndLoadModule(const QFileInfo &sourceCode, const QString &moduleName){
    auto moduleFilePath = sourceCode.absolutePath() + "/" + moduleName;
    auto result = compileToLibrary(sourceCode,moduleFilePath);
    if(result.first == 0){
        ModuleManager::singletone()->unloadLibrary(moduleFilePath);
        ModuleManager::singletone()->loadModule(ModuleManager::singletone()->getFreeAbsoluteFilePathForModule(moduleFilePath));
    }
    return result;
}


std::pair<int,QString> Compiler::compileToLibrary(const QFileInfo &file,const QString &absoluteNewLibraryFilePath){

    QString tempOutputFileName = absoluteNewLibraryFilePath+QString::number(rand());
    QProcess p;
#ifdef Q_OS_MAC
    //p.setEnvironment(QProcess::systemEnvironment()<<"PATH=/usr/bin:/bin:/usr/sbin:/sbin:/usr/local/bin:/Library/TeX/texbin:/usr/local/MacGPG2/bin:/usr/local/share/dotnet:/opt/X11/bin:/Library/Frameworks/Mono.framework/Versions/Current/Commands");
    QString cmd = compilerCmd + " "+  compilerLibraryFlags + " " + compilerFlags + " " + file.absoluteFilePath() + " -o " + tempOutputFileName  + " -I\"/"+includePath + "\" ";
    p.start("bash", QStringList() << "-c" << cmd);
#elif defined(Q_OS_WIN)
    QString tempName=tempOutputFileName + ".o";
    QString compilerCMD = compilerCmd.right(compilerCmd.length()-compilerCmd.lastIndexOf('/')-1);
    QString cmd = /*".\\" + */ compilerCMD + " -c \"" + file.absoluteFilePath() + "\" " + compilerFlags + " -o \"" + tempName+"\" -I\"" + includePath + "\" ";
    cmd = "\" " + cmd + " \"";
    qDebug().noquote() << cmd;
    /*
     * Use .\ ans setWorkingDir
     * OR
     * setEnviroment
     */
    //p.setWorkingDirectory(compilerCmd.left(compilerCmd.lastIndexOf('/')));

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("PATH", env.value("Path") + ";" +compilerCmd.left(compilerCmd.lastIndexOf('/')));
    p.setProcessEnvironment(env);
   // p.setEnvironment(QStringList() <<"PATH"<< compilerCmd.left(compilerCmd.lastIndexOf('/')));
    p.start("cmd /c " + cmd);
#endif
    p.waitForFinished();
    auto err = p.readAllStandardError();
    auto out = p.readAllStandardError();
    if(err.length()>0)qDebug().noquote() <<"ERR : "<< QString(err);
    if(out.length()>0)qDebug().noquote() <<"OuT : "<< QString(out);
    if(p.exitCode() != 0){
        qDebug() << p.errorString();
        qDebug() << p.error();
    }else{
#ifdef Q_OS_WIN
        // we only have a .o ans need a .dll
        // cmd: g++ -shared -o foo.dll foo.dll.o
        QProcess oToDll;
        oToDll.setEnvironment(QStringList() <<"PATH"<< compilerCmd.left(compilerCmd.lastIndexOf('/')));
        oToDll.start("cmd /c \" " + compilerCMD + " " + compilerLibraryFlags + " -o \"" + tempOutputFileName +"\"  \""+ tempName +"\" \"");
        oToDll.waitForFinished();
        QFile::rename(tempOutputFileName,absoluteNewLibraryFilePath);
        QFile::remove(tempName);
        return{oToDll.exitCode(),oToDll.readAllStandardError()};

#endif
#ifdef Q_OS_MAC
        if(p.exitCode()==0){
            Q_ASSERT(QFile::rename(tempOutputFileName,absoluteNewLibraryFilePath));
        }
#endif
    }
    return {p.exitCode(),QString(err)};
}

/**
 * @brief Compiler::compileToLibrary writes code into a file ans compile it, @see Compiler::compileToLibrary(QFileInfo,QString)
 * @param code The code to compile to a Module Library
 * @param newLibraryFile The filepath of the new generated library
 * @return return code from the compiler and the output of the compiler
 */
std::pair<int,QString> Compiler::compileToLibrary(const QString &code, const QString &newLibraryFile){
    QTemporaryFile file;
    file.open();
    {
        QTextStream out(&file);
        out << code;
    }
    file.close();
    return compileToLibrary(file,newLibraryFile);
}
}
