#include "zip.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <memory>

void unzipPowershellNew(const QFileInfo& zip, const QFileInfo& unzip, const std::function<void(bool)>& callback){
    // https://www.reddit.com/r/Windows10/comments/71z96c/unzip_from_command_line/
    // Expand-Archive "<PathToZIPFile>" "<FolderWhereToExtract>" -Force
    auto p = new QProcess();
    p->start(QStringLiteral("powershell.exe"), QStringList() << QStringLiteral("Expand-Archive") << QStringLiteral("-Force") << "\"" + zip.absoluteFilePath() + "\"" << "\"" + unzip.absoluteFilePath() + "\"");
    QObject::connect(p,qOverload<int,QProcess::ExitStatus>(&QProcess::finished),[p,zip,unzip,callback](auto exitCode, auto exitStatus){
        Q_UNUSED(exitStatus)
        if(exitCode != 0){
            qDebug() << "Failed to unzip " << zip << " to " << unzip << " with powershell new";
            qDebug().noquote() << "stderr : " << p->readAllStandardError();
            qDebug().noquote() << "stdout : " << p->readAllStandardOutput();
        }
        callback(p->error() == QProcess::ProcessError::UnknownError ? !exitCode : false);
        p->deleteLater();
    });
}
void unzipPowershell(const QFileInfo& zip, const QFileInfo& unzip, const std::function<void(bool)>& callback){
    // from https://stackoverflow.com/questions/17546016/how-can-you-zip-or-unzip-from-the-script-using-only-windows-built-in-capabiliti/26843122#26843122
    // powershell.exe -nologo -noprofile -command "& { Add-Type -A 'System.IO.Compression.FileSystem'; [IO.Compression.ZipFile]::ExtractToDirectory('foo.zip', 'bar'); }"
    auto p = new QProcess();
    p->start(QStringLiteral("powershell.exe"), QStringList() << QStringLiteral("-nologo") << QStringLiteral("-noprofile") << "& { Add-Type -A 'System.IO.Compression.FileSystem'; [IO.Compression.ZipFile]::ExtractToDirectory('"+zip.absoluteFilePath()+"', '"+unzip.absoluteFilePath()+"/unzippedDir'); }");
    QObject::connect(p,qOverload<int,QProcess::ExitStatus>(&QProcess::finished),[p,zip,unzip,callback](auto exitCode, auto exitStatus){
        Q_UNUSED(exitStatus)
        if(exitCode != 0){
            qDebug() << "Failed to unzip " << zip << " to " << unzip << " with powershell";
            qDebug().noquote() << "stderr : " << p->readAllStandardError();
            qDebug().noquote() << "stdout : " << p->readAllStandardOutput();
        }
        callback(p->error() == QProcess::ProcessError::UnknownError ? !exitCode : false);
        p->deleteLater();
    });
}

void unzipWinrar(const QFileInfo& zip, const QFileInfo& unzip, const std::function<void(bool)>& callback){
    // from https://stackoverflow.com/questions/1315662/how-to-extract-zip-files-with-winrar-command-line
    // "%ProgramFiles%\WinRAR\winrar.exe" x -ibck c:\file.zip *.* c:\folder
    auto p = new QProcess();
    p->start(QStringLiteral("C:\\Program Files\\WinRAR\\winrar.exe"), QStringList() << QStringLiteral("x") << QStringLiteral("-ibck") << QStringLiteral("-o+") << zip.absoluteFilePath() << QStringLiteral("*.*") << unzip.absoluteFilePath());
    QObject::connect(p,qOverload<int,QProcess::ExitStatus>(&QProcess::finished),[p,zip,unzip,callback](auto exitCode, auto exitStatus){
        Q_UNUSED(exitStatus)
        if(exitCode != 0){
            qDebug() << "Failed to unzip " << zip << " to " << unzip << " with winrar";
            qDebug().noquote() << "stderr : " << p->readAllStandardError();
            qDebug().noquote() << "stdout : " << p->readAllStandardOutput();
        }
        callback(p->error() == QProcess::ProcessError::UnknownError ? !exitCode : false);
        p->deleteLater();
    });
}

void unzip7Zip(const QFileInfo& zip, const QFileInfo& unzip, const std::function<void(bool)>& callback){
    // from https://superuser.com/questions/95902/7-zip-and-unzipping-from-command-line
    // 7z x example.zip -oexample
    auto p = new QProcess();
    p->start(QStringLiteral("C:\\Program Files\\7-Zip\\7z.exe"), QStringList() << QStringLiteral("x") << QStringLiteral("-y") << zip.absoluteFilePath() << "-o" + unzip.absoluteFilePath());
    QObject::connect(p,qOverload<int,QProcess::ExitStatus>(&QProcess::finished),[p,zip,unzip,callback](auto exitCode, auto exitStatus){
        Q_UNUSED(exitStatus)
        if(exitCode != 0){
            qDebug() << "Failed to unzip " << zip << " to " << unzip << " with 7zip";
            qDebug().noquote() << "stderr : " << p->readAllStandardError();
            qDebug().noquote() << "stdout : " << p->readAllStandardOutput();
        }
        callback(p->error() == QProcess::ProcessError::UnknownError ? !exitCode : false);
        p->deleteLater();
    });
}

void Zip::unzip(const QFileInfo& zip, const QFileInfo& unzip, const std::function<void(bool)>& callback){
    if(!zip.exists()){
        return;
    }
    QDir().mkpath(unzip.absoluteFilePath());
    unzipWinrar(zip,unzip,[=](bool success){
        if (success) {
            callback(true);
        } else {
            unzip7Zip(zip,unzip,[=](bool success){
                if (success) {
                    callback(true);
                } else {
                    unzipPowershellNew(zip,unzip,[=](bool success){
                        if (success) {
                            callback(true);
                        } else {
                            unzipPowershell(zip,unzip,[=](bool success){
                                callback(success);
                            });
                        }
                    });
                }
            });
        }
    });
}
