#include "updater.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFuture>
#include <QFutureWatcher>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QSysInfo>
#include <QTemporaryFile>
#include <QUrl>
#include <QVersionNumber>
#include <QtConcurrent/QtConcurrentRun>
#include <zip/zip.h>

QByteArray getFileContent(const QString &filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file " << filename;
        return {"Failed to open file"};
    }
    return file.readAll();
}

QVersionNumber stringToVersion(QAnyStringView version) {
    while (version.length() > 0 && !version.front().isNumber()) {
        version = version.mid(1);
    }
    return QVersionNumber::fromString(version);
}

QString getOSName() {

#if defined(Q_OS_WIN)
    return "windows";
#elif defined(Q_OS_LINUX)
    return "linux";
#elif defined(Q_OS_MAC)
    return "macos";
#else
    return "unknown";
#endif
}

QString getArchName() {
    QString arch = QSysInfo::buildCpuArchitecture();

    // Map to conventional architecture namesÏ
    if (arch == "x86_64" || arch == "amd64") {
        arch = "x64";
    } else if (arch == "i386" || arch == "i686") {
        arch = "x86";
    } else if (arch == "arm" || arch == "armv7l") {
        arch = "arm32";
    } else if (arch == "aarch64") {
        arch = "arm64";
    } else {
        arch = "unknown";
    }
    return arch;
}

Updater::Updater() {
    if (!QFile::exists(QDir::currentPath() + "/Lichtsteuerung.exe")) {
        state = UpdaterState::IDE_ENV;
        emit stateChanged();
    }
}

QFuture<bool> extractZip(const QFile &file) {
    return QtConcurrent::run(
        [path = QFileInfo(file).absoluteFilePath().toStdString(), dest = QFileInfo(file).absolutePath().toStdString()] { return zip_extract(path.c_str(), dest.c_str(), nullptr, nullptr) == 0; });
}

void Updater::checkForUpdate() {
    if (state == UpdaterState::IDE_ENV) {
        return;
    }
    if (!QFile::exists(VERSION_FILE_NAME)) {
        qDebug() << "version file does not exists in application folder";
        state = UpdaterState::NotChecked;
        emit stateChanged();
        return;
    }
    QNetworkRequest request{QUrl(latestGithubReleaseURL)};
    request.setRawHeader("Accept", "application/vnd.github.v3+json");
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    auto response = http->get(request);
    QObject::connect(response, &QNetworkReply::finished, [this, response]() {
        response->deleteLater();
        if (response->attribute(QNetworkRequest::HttpStatusCodeAttribute) != 200) {
            state = NoUpdateAvailible;
            emit stateChanged();
            return;
        }

        // Parse the JSON response
        QJsonDocument jsonResponse = QJsonDocument::fromJson(response->readAll());
        QJsonObject jsonObject = jsonResponse.object();
        QJsonArray assets = jsonObject["assets"].toArray();
        if (assets.isEmpty()) {
            state = NoUpdateAvailible;
            emit stateChanged();
            qDebug() << "No assets found!";
            return;
        }

        QVersionNumber ownVersion = stringToVersion(getFileContent(VERSION_FILE_NAME));
        QVersionNumber githubVersion = stringToVersion(jsonObject["tag_name"].toString());
        if (githubVersion.isNull() || ownVersion.isNull()) {
            state = NoUpdateAvailible;
            emit stateChanged();
            qDebug() << "Failed to parse versions. Own: " << ownVersion << " github: " << githubVersion;
            return;
        }

        if (githubVersion <= ownVersion) {
            state = NoUpdateAvailible;
            emit stateChanged();
            return;
        }

        const auto os = getOSName();
        const auto arch = getArchName();

        assetDownloadUrl.clear();
        for (const QJsonValue &assetV : assets) {
            auto asset = assetV.toObject();
            QString assetName = asset["name"].toString();
            if (assetName.contains(os) && assetName.contains(arch)) {
                assetDownloadUrl = asset["browser_download_url"].toString();
                break;
            }
        }

        if (assetDownloadUrl.isEmpty()) {
            state = NoUpdateAvailible;
            emit stateChanged();
            qDebug() << "No Matching Asset Found for" << os << "-" << arch;
            return;
        }

        state = UpdaterState::UpdateAvailible;
        emit stateChanged();
        emit needUpdate();
    });
}

void Updater::update() {
    if (state != UpdaterState::UpdateAvailible) {
        qDebug() << "we are not in a state to make updates";
        return;
    }
    state = UpdaterState::DownloadingUpdate;
    emit stateChanged();
    QNetworkRequest request{QUrl(assetDownloadUrl)};
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    auto response = http->get(request);
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
        QFuture<bool> future = extractZip(*deploy);
        watcher->setFuture(future);
    });
}

void Updater::runUpdateInstaller() {
    if (state != UpdaterState::ReadyToInstall) {
        return;
    }
    QString from = deployPath;
    QString to = QFileInfo(QDir::currentPath()).absoluteFilePath();
    QString installer = deployPath.left(deployPath.lastIndexOf(QLatin1String("/"))) + "/" + WINDOWS_INSTALLER_NAME;
    QProcess::startDetached(installer, QStringList() << from << to);
}
