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


/**
 * @brief Compiler::compileToLibrary compiles the code in the file to a module Library
 * @note Unloads the old Library found at newLibraryFile and loads the new Library, replacing all
 *       old Programs/Filters/Consumers in ProgramBlockManager by new in instances
 * @param file a FileInfo Object describing the file where the code is in
 * @param newLibraryFile the FilePath of the new Library
 * @return return code from the compiler and the output of the compiler
 */
std::pair<int,QString> Compiler::compileToLibrary(const QFileInfo &file,const QString &newLibraryFile){
    QProcess p;
#ifdef Q_OS_MAC
    p.setEnvironment(QProcess::systemEnvironment()<<"PATH=/usr/bin:/bin:/usr/sbin:/sbin:/usr/local/bin:/Library/TeX/texbin:/usr/local/MacGPG2/bin:/usr/local/share/dotnet:/opt/X11/bin:/Library/Frameworks/Mono.framework/Versions/Current/Commands");
    QString cmd = compilerCmd + " "+  compilerLibraryFlags + " " + compilerFlags + " " + file.absoluteFilePath() + " -o " + file.absolutePath()+"/"+newLibraryFile + " -I\"/"+includePath + "\" ";
    p.start("bash", QStringList() << "-c" << cmd);
#elif defined(Q_OS_WIN)
    QString tempName=newLibraryFile + ".o";
    QString compilerCMD = compilerCmd.right(compilerCmd.length()-compilerCmd.lastIndexOf('/')-1);
    QString cmd = /*".\\" + */ compilerCMD + " -c \"" + file.absoluteFilePath() + "\" " + compilerFlags + " -o \"" + file.absolutePath() + "/" + tempName+"\" -I\"" + includePath + "\" ";
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
        //qDebug().noquote() << cmd;
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
#ifdef Q_OS_MAC
        // we have to unload the existing lib first
        if(p.exitCode()==0){
            ModuleManager::singletone()->singletone()->unloadLibrary(file.absolutePath() + "/" + newLibraryFile);
            ModuleManager::singletone()->singletone()->loadModule(file.absolutePath() + "/" + newLibraryFile);
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
