#ifndef UPDATER_H
#define UPDATER_H

#include <QNetworkAccessManager>
#include <QObject>
#include <QString>
#include <memory>

class Updater : public QObject{
    Q_OBJECT
    Q_PROPERTY(int progress READ getUpdateProgress NOTIFY updateProgressChanged)
    const inline static QString VERSION_FILE_NAME = QStringLiteral("version.txt");
    const inline static QString NAME_OF_DEPLOY_FOLDER = QStringLiteral("windows-release-5.12.3");
    QString versionDownloadURL = QStringLiteral("https://git.rwth-aachen.de/leander.schulten/Lichtsteuerung/-/jobs/artifacts/windows-release/download?job=version");
    QString deployDownloadURL = QStringLiteral("https://git.rwth-aachen.de/leander.schulten/Lichtsteuerung/-/jobs/artifacts/windows-release/download?job=deploy");
    std::unique_ptr<QNetworkAccessManager> http = std::make_unique<QNetworkAccessManager>();
    enum class UpdaterState {IDE_ENV, NotChecked, NoUpdateAvailible, UpdateAvailible, DownloadingUpdate, UpdateDownloaded, DownloadUpdateFailed} state = UpdaterState::NotChecked;
    int progress = -1;
    QString deployPath;
public:
    Updater();
    void checkForUpdate();
    UpdaterState getState()const {return state;}
    void update();
    /**
     * @brief runUpdateInstaller starts the installer script, call when the application is closing
     */
    void runUpdateInstaller();
    int getUpdateProgress()const{return progress;}
signals:
    void needUpdate();
    void updateProgressChanged();
};

#endif // UPDATER_H