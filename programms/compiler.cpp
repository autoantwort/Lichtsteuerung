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


Compiler::Compiler()
{

}


std::pair<int,QString> Compiler::compileToLibrary(const QFileInfo &file,const QString &newLibraryFile){
    QProcess p;
#ifdef Q_OS_MAC
    p.setEnvironment(QProcess::systemEnvironment()<<"PATH=/usr/bin:/bin:/usr/sbin:/sbin:/usr/local/bin:/Library/TeX/texbin:/usr/local/MacGPG2/bin:/usr/local/share/dotnet:/opt/X11/bin:/Library/Frameworks/Mono.framework/Versions/Current/Commands");
    p.start("bash", QStringList() << "-c" << compilerCmd + " "+  compilerLibraryFlags + " " + compilerFlags + " " + file.fileName() + " -o " + newLibraryFile);
#elif defined(Q_OS_WIN)
    QString tempName=newLibraryFile + ".o";
    QString compilerCMD = compilerCmd.right(compilerCmd.length()-compilerCmd.lastIndexOf('/')-1);
    QString cmd = /*".\\" + */ compilerCMD + " -c \"" + file.absoluteFilePath() + "\" " + compilerFlags + " -o \"" + file.absolutePath() + "/" + tempName+"\" -I\"" + QDir::currentPath() + "\" ";
    cmd = "\" " + cmd + " \"";
    qDebug().noquote() << cmd;
    /*
     * Use .\ ans setWorkingDir
     * OR
     * setEnviroment
     */
    //p.setWorkingDirectory(compilerCmd.left(compilerCmd.lastIndexOf('/')));
    p.setEnvironment(QStringList() <<"PATH"<< compilerCmd.left(compilerCmd.lastIndexOf('/')));
    p.start("cmd /c " + cmd);
#endif
    p.waitForFinished();
    auto err = p.readAllStandardError();
    auto out = p.readAllStandardError();
    if(err.length()>0)qDebug().noquote() <<"ERR : "<< QString(err);
    if(out.length()>0)qDebug().noquote() <<"OuT : "<< QString(out);
    if(p.exitStatus() == QProcess::CrashExit){
        qDebug() << p.errorString();
        qDebug() << p.error();
        qDebug().noquote() << cmd;
    }else{
#ifdef Q_OS_WIN
        // we have to unload the existing lib first
        ModuleManager::singletone()->singletone()->unloadLibrary(file.absolutePath() + "/" + newLibraryFile);
        // we only have a .o ans need a .dll
        // cmd: g++ -shared -o foo.dll foo.dll.o
        QProcess oToDll;
        oToDll.setEnvironment(QStringList() <<"PATH"<< compilerCmd.left(compilerCmd.lastIndexOf('/')));
        oToDll.start("cmd /c \" " + compilerCMD + " " + compilerLibraryFlags + " -o \"" + file.absolutePath() + "/" + newLibraryFile +"\"  \""+ file.absolutePath() + "/" + tempName +"\" \"");
        oToDll.waitForFinished();
        if(oToDll.exitCode() == 0){
            ModuleManager::singletone()->singletone()->loadModule(file.absolutePath() + "/" + newLibraryFile);
        }
        return{oToDll.exitCode(),oToDll.readAllStandardError()};

#endif
    }
    return {p.exitCode(),QString(err)};
}

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
