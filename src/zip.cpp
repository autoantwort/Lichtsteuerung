#include "zip.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>

bool unzipPowershellNew(const QFileInfo& zip, const QFileInfo& unzip){
    // https://www.reddit.com/r/Windows10/comments/71z96c/unzip_from_command_line/
    // Expand-Archive "<PathToZIPFile>" "<FolderWhereToExtract>" -Force
    QProcess p;
    p.start(QStringLiteral("powershell.exe"), QStringList() << QStringLiteral("Expand-Archive") << QStringLiteral("-Force") << "\"" + zip.absoluteFilePath() + "\"" << "\"" + unzip.absoluteFilePath() + "\"");
    p.waitForFinished();
    if(p.exitCode() != 0){
        qDebug() << "Failed to unzip " << zip << " to " << unzip << " with powershell new";
        qDebug().noquote() << "stderr : " << p.readAllStandardError();
        qDebug().noquote() << "stdout : " << p.readAllStandardOutput();
    }
    return p.error() == QProcess::ProcessError::UnknownError ? !p.exitCode() : false;
}
bool unzipPowershell(const QFileInfo& zip, const QFileInfo& unzip){
    // from https://stackoverflow.com/questions/17546016/how-can-you-zip-or-unzip-from-the-script-using-only-windows-built-in-capabiliti/26843122#26843122
    // powershell.exe -nologo -noprofile -command "& { Add-Type -A 'System.IO.Compression.FileSystem'; [IO.Compression.ZipFile]::ExtractToDirectory('foo.zip', 'bar'); }"
    QProcess p;
    p.start(QStringLiteral("powershell.exe"), QStringList() << QStringLiteral("-nologo") << QStringLiteral("-noprofile") << "& { Add-Type -A 'System.IO.Compression.FileSystem'; [IO.Compression.ZipFile]::ExtractToDirectory('"+zip.absoluteFilePath()+"', '"+unzip.absoluteFilePath()+"/unzippedDir'); }");
    p.waitForFinished();
    if(p.exitCode() != 0){
        qDebug() << "Failed to unzip " << zip << " to " << unzip << " with powershell";
        qDebug().noquote() << "stderr : " << p.readAllStandardError();
        qDebug().noquote() << "stdout : " << p.readAllStandardOutput();
    }
    return p.error() == QProcess::ProcessError::UnknownError ? !p.exitCode() : false;
}

bool unzipWinrar(const QFileInfo& zip, const QFileInfo& unzip){
    // from https://stackoverflow.com/questions/1315662/how-to-extract-zip-files-with-winrar-command-line
    // "%ProgramFiles%\WinRAR\winrar.exe" x -ibck c:\file.zip *.* c:\folder
    QProcess p;
    p.start(QStringLiteral("C:\\Program Files\\WinRAR\\winrar.exe"), QStringList() << QStringLiteral("x") << QStringLiteral("-ibck") << QStringLiteral("-o+") << zip.absoluteFilePath() << QStringLiteral("*.*") << unzip.absoluteFilePath());
    p.waitForFinished();
    if(p.exitCode() != 0){
        qDebug() << "Failed to unzip " << zip << " to " << unzip << " with winrar";
        qDebug().noquote() << "stderr : " << p.readAllStandardError();
        qDebug().noquote() << "stdout : " << p.readAllStandardOutput();
    }
    return p.error() == QProcess::ProcessError::UnknownError ? !p.exitCode() : false;
}

bool unzip7Zip(const QFileInfo& zip, const QFileInfo& unzip){
    // from https://superuser.com/questions/95902/7-zip-and-unzipping-from-command-line
    // 7z x example.zip -oexample
    QProcess p;
    p.start(QStringLiteral("C:\\Program Files\\7-Zip\\7z.exe"), QStringList() << QStringLiteral("x") << QStringLiteral("-y") << zip.absoluteFilePath() << "-o" + unzip.absoluteFilePath());
    p.waitForFinished();
    if(p.exitCode() != 0){
        qDebug() << "Failed to unzip " << zip << " to " << unzip << " with 7zip";
        qDebug().noquote() << "stderr : " << p.readAllStandardError();
        qDebug().noquote() << "stdout : " << p.readAllStandardOutput();
    }
    return p.error() == QProcess::ProcessError::UnknownError ? !p.exitCode() : false;
}

bool Zip::unzip(const QFileInfo& zip, const QFileInfo& unzip){
    if(!zip.exists()){
        return false;
    }
    QDir().mkpath(unzip.absoluteFilePath());
    if(unzipWinrar(zip,unzip)){
        return true;
    }
    if(unzip7Zip(zip,unzip)){
        return true;
    }
    if(unzipPowershellNew(zip,unzip)){
        return true;
    }
    if(unzipPowershell(zip,unzip)){
        return true;
    }
    return false;
}
