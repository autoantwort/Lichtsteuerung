#include "compiler.h"
#include <QTemporaryFile>
#include <QTextStream>
#include <QProcess>
#include <QDebug>
#include <iostream>
#include <QDir>
namespace Modules{



QString Compiler::compilerCmd = "g++";
QString Compiler::compilerLibraryFlags = "-shared -fPIC";
QString Compiler::compilerFlags = "-std=c++14 -O3";


Compiler::Compiler()
{

}


int Compiler::compileToLibrary(const QFile &file,const QString &newLibraryFile){
    QProcess p;
#ifdef Q_OS_MAC
    p.setEnvironment(QProcess::systemEnvironment()<<"PATH=/usr/bin:/bin:/usr/sbin:/sbin:/usr/local/bin:/Library/TeX/texbin:/usr/local/MacGPG2/bin:/usr/local/share/dotnet:/opt/X11/bin:/Library/Frameworks/Mono.framework/Versions/Current/Commands");
    p.start("bash", QStringList() << "-c" << compilerCmd + " "+  compilerLibraryFlags + " " + compilerFlags + " " + file.fileName() + " -o " + newLibraryFile);
#elif defined(Q_OS_WIN)
    QString cmd = /*"cd " + compilerCmd.left(compilerCmd.lastIndexOf('/')) + " && " +*/ compilerCmd.right(compilerCmd.length()-compilerCmd.lastIndexOf('/')-1) + " -c \"" + file.fileName() + "\" "+  compilerLibraryFlags + " " + compilerFlags + " -o \"" + newLibraryFile+"\" -I\"" + QDir::currentPath() + "\"";
    p.setWorkingDirectory(compilerCmd.left(compilerCmd.lastIndexOf('/')));
    //p.start("cmd",QStringList()<< "/c" << cmd);
    p.start(cmd);
    qDebug() << cmd;
    std::cout << cmd.toStdString() << '\n';
    qDebug() << compilerCmd.right(compilerCmd.length()-compilerCmd.lastIndexOf('/')-1);
    qDebug() << compilerCmd;

#endif
    p.waitForFinished();
    auto err = p.readAllStandardError();
    auto out = p.readAllStandardError();
    qDebug() << err;
    qDebug() << out;
    std::cout << err.toStdString() << '\n';
    std::cout << out.toStdString() << '\n';
    qDebug() << p.errorString();
    qDebug() << p.error();
    qDebug() << p.exitStatus();
    qDebug() << p.exitCode();
    return p.exitCode();
}

int Compiler::compileToLibrary(const QString &code, const QString &newLibraryFile){
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
