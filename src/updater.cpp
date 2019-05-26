#include "updater.h"

#include "zip.h"
#include <QDir>
#include <QFileInfo>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QTemporaryFile>
#include <thread>


Updater::Updater(){
    if(!QFile::exists(QDir::currentPath() + "/Lichtsteuerung.exe")){
        state = UpdaterState::IDE_ENV;
    }
}

void Updater::checkForUpdate(){
    if(state == UpdaterState::IDE_ENV){
        return;
    }
    if(!QFile::exists(VERSION_FILE_NAME)){
        qDebug() << "version file does not exists in application folder";
        state = UpdaterState::UpdateAvailible;
        emit needUpdate();
        return;
    }
    auto redirect = http->get(QNetworkRequest(QUrl(versionDownloadURL)));
    QObject::connect(redirect,&QNetworkReply::finished,[this,redirect](){
        redirect->deleteLater();
        auto redirectURL = redirect->header(QNetworkRequest::KnownHeaders::LocationHeader);
        if(!redirectURL.isValid()){
            qDebug() << "can not redirect version.zip";
            return;
        }
        auto response = http->get(QNetworkRequest(redirectURL.toUrl()));
        QObject::connect(response,&QNetworkReply::finished,[this,response](){
            std::thread thread([this,response](){

                QFile version(QDir::tempPath() + QStringLiteral("/version.zip"));
                version.open(QFile::WriteOnly);
                version.write(response->readAll());
                version.close();
                response->deleteLater();
                if(!Zip::unzip(QFileInfo(version),QFileInfo(version).absolutePath())){
                    qDebug() << "not successful when unzipping";
                    state = UpdaterState::NoUpdateAvailible;
                    return;
                }
                if(!QFile::exists(QFileInfo(version).absolutePath() + "/" + VERSION_FILE_NAME)){
                    qDebug() << "version file does not exists in version.zip";
                    state = UpdaterState::NoUpdateAvailible;
                    return;
                }
                if(QFile(VERSION_FILE_NAME).readAll() != QFile(QFileInfo(version).absolutePath() + "/" + VERSION_FILE_NAME).readAll()){
                    state = UpdaterState::UpdateAvailible;
                    emit needUpdate();
                }else{
                    state = UpdaterState::NoUpdateAvailible;
                }

            });
            thread.detach();
        });
    });
}

void Updater::update(){
    if(state != UpdaterState::UpdateAvailible) {
        qDebug() << "we are not in a state to make updates";
        return;
    }
    qDebug() << "Update";
    state = UpdaterState::DownloadingUpdate;
    auto redirect = http->get(QNetworkRequest(QUrl(deployDownloadURL)));
    QObject::connect(redirect,static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),[this,redirect](auto error){
        qWarning() << "Error while redirecting to deploy.zip! " << error << redirect->errorString();
        redirect->deleteLater();
        state = UpdaterState::DownloadUpdateFailed;
    });
    QObject::connect(redirect,&QNetworkReply::finished,[this,redirect](){
        redirect->deleteLater();
        auto redirectURL = redirect->header(QNetworkRequest::KnownHeaders::LocationHeader);
        if(!redirectURL.isValid()){
            qDebug() << "can not redirect deploy.zip";
            return;
        }
        auto response = http->get(QNetworkRequest(redirectURL.toUrl()));
        QFile * deploy = new QFile(QDir::tempPath() + QStringLiteral("/deploy.zip"));
        deploy->open(QFile::WriteOnly);
        QObject::connect(response,static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),[this,response,deploy](auto error){
            qWarning() << "Error while downloading deploy.zip! " << error << response->errorString();
            deploy->close();
            deploy->deleteLater();
            response->deleteLater();
            state = UpdaterState::DownloadUpdateFailed;
        });
        QObject::connect(response,&QNetworkReply::readyRead,[response,deploy](){
            deploy->write(response->readAll());
        });
        QObject::connect(response,&QNetworkReply::downloadProgress,[this](auto rec, auto max){
            progress = 100 * rec / max;
            qDebug() << progress;
            emit updateProgressChanged();
        });
        QObject::connect(response,&QNetworkReply::finished,[this,response,deploy](){
            std::thread thread([this,response,deploy](){
                qDebug() << response->atEnd();
                deploy->close();
                std::unique_ptr<QFile> deleteMe(deploy);
                response->deleteLater();
                if(!Zip::unzip(QFileInfo(*deploy),QFileInfo(*deploy).absolutePath())){
                    qDebug() << "not successful when unzipping deploy.zip";
                    state = UpdaterState::DownloadUpdateFailed;
                    return;
                }
                deployPath = QFileInfo(*deploy).absolutePath() + "/" + NAME_OF_DEPLOY_FOLDER;
                if(!QFile::rename(deployPath + "/" + WINDOWS_INSTALLER_NAME,QFileInfo(*deploy).absolutePath() + "/" + WINDOWS_INSTALLER_NAME)){
                    qWarning() << "Failed to rename Windows Installer";
                    state = UpdaterState::DownloadUpdateFailed;
                    return;
                }
                // all important files like QTJSONFile.json
                auto entries = QDir(QDir::currentPath()).entryInfoList(QStringList() << QStringLiteral("QTJSONFile.json*"),QDir::Filter::Files);
                for(const auto & e : entries){
                    QFile::copy(e.absoluteFilePath() , deployPath + "/" + e.fileName());
                }
                state = UpdaterState::UpdateDownloaded;
            });
            thread.detach();
        });
    });

}

void Updater::runUpdateInstaller(){
    if(state != UpdaterState::UpdateDownloaded){
        return;
    }
    QString from = deployPath;
    QString to = QFileInfo(QDir::currentPath()).absoluteFilePath();
    QString installer = deployPath.left(deployPath.lastIndexOf(QLatin1String("/"))) + "/" + WINDOWS_INSTALLER_NAME;
    QProcess::startDetached(installer, QStringList() << from << to);
}
