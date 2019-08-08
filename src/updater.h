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
    const inline static QString NAME_OF_DEPLOY_FOLDER = QStringLiteral("windows-release-5.13.0");
    const inline static QString WINDOWS_INSTALLER_NAME = QStringLiteral("WindowsInstaller.exe");
    QString versionDownloadURL = QStringLiteral("https://git.rwth-aachen.de/leander.schulten/Lichtsteuerung/-/jobs/artifacts/windows-release/download?job=version");
    QString deployDownloadURL = QStringLiteral("https://git.rwth-aachen.de/leander.schulten/Lichtsteuerung/-/jobs/artifacts/windows-release/download?job=deploy");
    std::unique_ptr<QNetworkAccessManager> http = std::make_unique<QNetworkAccessManager>();
public:
    enum class UpdaterState {IDE_ENV, NotChecked, NoUpdateAvailible, UpdateAvailible, DownloadingUpdate, UpdateDownloaded, DownloadUpdateFailed};
private:
    UpdaterState state = UpdaterState::NotChecked;
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
    QNetworkAccessManager * getQNetworkAccessManager(){return http.get();}
signals:
    void needUpdate();
    void updateProgressChanged();
    void stateChanged();
};

#endif // UPDATER_H
