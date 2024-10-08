#ifndef UPDATER_H
#define UPDATER_H

#include <QNetworkAccessManager>
#include <QObject>
#include <QString>
#include <memory>

class Updater : public QObject {
    Q_OBJECT
    Q_PROPERTY(int progress READ getUpdateProgress NOTIFY updateProgressChanged)
    Q_PROPERTY(UpdaterState state READ getState NOTIFY stateChanged)
    const inline static QString VERSION_FILE_NAME = QStringLiteral("version.txt");
    const inline static QString NAME_OF_DEPLOY_FOLDER = QStringLiteral("windows-artifact");
    const inline static QString WINDOWS_INSTALLER_NAME = QStringLiteral("WindowsInstaller.exe");
    QString latestGithubReleaseURL = QStringLiteral("https://api.github.com/repos/autoantwort/Lichtsteuerung/releases/latest");
    std::unique_ptr<QNetworkAccessManager> http = std::make_unique<QNetworkAccessManager>();

public:
    enum UpdaterState {
        IDE_ENV,
        NotChecked,
        NoUpdateAvailible,
        UpdateAvailible,
        DownloadingUpdate,
        UnzippingUpdate,
        UnzippingFailed,
        PreparationForInstallationFailed,
        ReadyToInstall,
        DownloadUpdateFailed
    };
    Q_ENUM(UpdaterState)
private:
    UpdaterState state = UpdaterState::NotChecked;
    int progress = -1;
    QString deployPath;
    QString assetDownloadUrl;

public:
    Updater();
    void checkForUpdate();
    UpdaterState getState() const { return state; }
    void update();
    /**
     * @brief runUpdateInstaller starts the installer script, call when the application is closing
     */
    void runUpdateInstaller();
    int getUpdateProgress() const { return progress; }
    QNetworkAccessManager *getQNetworkAccessManager() { return http.get(); }
signals:
    void needUpdate();
    void updateProgressChanged();
    void stateChanged();
};

#endif // UPDATER_H
