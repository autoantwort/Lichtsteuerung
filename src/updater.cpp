#include "updater.h"

#include <QDir>
#include <QFileInfo>
#include <QFuture>
#include <QFutureWatcher>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QTemporaryFile>
#include <QtConcurrent/QtConcurrentRun>
#include <quazip/JlCompress.h>

QByteArray getFileContent(const QString & filename){
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)){
        qWarning() <<  "Failed to open file " << filename;
        return {"Failed to open file"};
    }
    return file.readAll();
}

Updater::Updater(){
    if(!QFile::exists(QDir::currentPath() + "/Lichtsteuerung.exe")){
        state = UpdaterState::IDE_ENV;
        emit stateChanged();
    }
}

void Updater::checkForUpdate(){
    if(state == UpdaterState::IDE_ENV){
        return;
    }
    if (!QFile::exists(VERSION_FILE_NAME)) {
        qDebug() << "version file does not exists in application folder";
        state = UpdaterState::UpdateAvailible;
        emit stateChanged();
        emit needUpdate();
        return;
    }
    QNetworkRequest request{QUrl(versionDownloadURL)};
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    auto response = http->get(request);
    QObject::connect(response, &QNetworkReply::finished, [this, response]() {
        response->deleteLater();
        if (response->attribute(QNetworkRequest::HttpStatusCodeAttribute) != 200) {
            state = NoUpdateAvailible;
            emit stateChanged();
            return;
        }
        QFile version(QDir::tempPath() + QStringLiteral("/version.zip"));
        version.open(QFile::WriteOnly);
        version.write(response->readAll());
        version.close();

        auto watcher = new QFutureWatcher<bool>;
        connect(watcher, &QFutureWatcher<bool>::finished, [this, version = QFileInfo(version), watcher]() {
            watcher->deleteLater();
            bool success = watcher->future().result();
            if (!success) {
                qDebug() << "not successful when unzipping version.zip";
                state = UpdaterState::NoUpdateAvailible;
                emit stateChanged();
                return;
            }
            if (!QFile::exists(version.absolutePath() + "/" + VERSION_FILE_NAME)) {
                qDebug() << "version file does not exists in version.zip";
                state = UpdaterState::NoUpdateAvailible;
                emit stateChanged();
                return;
            }
            if (getFileContent(VERSION_FILE_NAME) != getFileContent(version.absolutePath() + "/" + VERSION_FILE_NAME)) {
                state = UpdaterState::UpdateAvailible;
                emit stateChanged();
                emit needUpdate();
            } else {
                state = UpdaterState::NoUpdateAvailible;
                emit stateChanged();
            }
        });

        // Start the computation.
        QFuture<bool> future = QtConcurrent::run([path = QFileInfo(version).absoluteFilePath(), dest = QFileInfo(version).absolutePath()] { return !JlCompress::extractDir(path, dest).empty(); });
        watcher->setFuture(future);
    });
}

void Updater::update(){
    if(state != UpdaterState::UpdateAvailible) {
        qDebug() << "we are not in a state to make updates";
        return;
    }
    state = UpdaterState::DownloadingUpdate;
    emit stateChanged();
    QNetworkRequest request{QUrl(deployDownloadURL)};
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    auto response = http->get(request);
    QObject::connect(response, &QNetworkReply::errorOccurred, [this, response](auto error) {
        qWarning() << "Error while redirecting to deploy.zip! " << error << response->errorString();
        response->deleteLater();
        state = UpdaterState::DownloadUpdateFailed;
        emit stateChanged();
    });
    QFile *deploy = new QFile(QDir::tempPath() + QStringLiteral("/deploy.zip"));
    deploy->open(QFile::WriteOnly);
    QObject::connect(response, &QNetworkReply::errorOccurred, [this, response, deploy](auto error) {
        qWarning() << "Error while downloading deploy.zip! " << error << response->errorString();
        deploy->close();
        deploy->deleteLater();
        response->deleteLater();
        state = UpdaterState::DownloadUpdateFailed;
        emit stateChanged();
    });
    QObject::connect(response, &QNetworkReply::readyRead, [response, deploy]() { deploy->write(response->readAll()); });
    QObject::connect(response, &QNetworkReply::downloadProgress, [this](auto rec, auto max) {
        progress = 100 * rec / max;
        emit updateProgressChanged();
    });
    QObject::connect(response, &QNetworkReply::finished, [this, response, deploy]() {
        qDebug() << response->atEnd();
        response->deleteLater();
        deploy->close();
        if (response->attribute(QNetworkRequest::HttpStatusCodeAttribute) != 200) {
            state = DownloadUpdateFailed;
            emit stateChanged();
            return;
        }
        state = UpdaterState::UnzippingUpdate;
        emit stateChanged();
        auto watcher = new QFutureWatcher<bool>;
        connect(watcher, &QFutureWatcher<bool>::finished, [this, deploy, watcher]() {
            watcher->deleteLater();
            bool success = watcher->future().result();
            std::unique_ptr<QFile> deleteMe(deploy);
            if (!success) {
                qDebug() << "not successful when unzipping deploy.zip";
                state = UpdaterState::UnzippingFailed;
                emit stateChanged();
                return;
            }
            deployPath = QFileInfo(*deploy).absolutePath() + "/" + NAME_OF_DEPLOY_FOLDER;
            // if old installer exists, delete these
            QString targetInstallerPath = QFileInfo(*deploy).absolutePath() + "/" + WINDOWS_INSTALLER_NAME;
            if (QFile::exists(targetInstallerPath)) {
                if (!QFile::remove(targetInstallerPath)) {
                    qWarning() << "Failed to remove old Windows Installer";
                    state = UpdaterState::PreparationForInstallationFailed;
                    emit stateChanged();
                    return;
                }
            }
            if (!QFile::rename(deployPath + "/" + WINDOWS_INSTALLER_NAME, targetInstallerPath)) {
                qWarning() << "Failed to rename Windows Installer";
                state = UpdaterState::PreparationForInstallationFailed;
                emit stateChanged();
                return;
            }
            // all important files like QTJSONFile.json
            auto entries = QDir(QDir::currentPath()).entryInfoList(QStringList() << QStringLiteral("QTJSONFile.json*"), QDir::Filter::Files);
            for (const auto &e : entries) {
                QFile::copy(e.absoluteFilePath(), deployPath + "/" + e.fileName());
            }
            state = UpdaterState::ReadyToInstall;
            emit stateChanged();
        });

        // Start the computation.
        QFuture<bool> future = QtConcurrent::run([path = QFileInfo(*deploy).absoluteFilePath(), dest = QFileInfo(*deploy).absolutePath()] { return !JlCompress::extractDir(path, dest).empty(); });
        watcher->setFuture(future);
    });
}

void Updater::runUpdateInstaller(){
    if(state != UpdaterState::ReadyToInstall){
        return;
    }
    QString from = deployPath;
    QString to = QFileInfo(QDir::currentPath()).absoluteFilePath();
    QString installer = deployPath.left(deployPath.lastIndexOf(QLatin1String("/"))) + "/" + WINDOWS_INSTALLER_NAME;
    QProcess::startDetached(installer, QStringList() << from << to);
}
