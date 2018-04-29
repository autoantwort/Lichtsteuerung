#include "compiler.h"
#include <QTemporaryFile>
#include <QTextStream>
#include <QProcess>
#include <QDebug>
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
#error Not Implemented
#endif
    p.waitForFinished();
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
