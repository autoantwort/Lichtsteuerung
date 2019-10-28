#ifndef SETTINGSFILEWRAPPER_H
#define SETTINGSFILEWRAPPER_H

#include "settings.h"
#include <QObject>

/**
 * @brief The SettingsFileWrapper class wrapps the status of the settings file for QML
 */
class SettingsFileWrapper : public QObject {
    Q_OBJECT
public:
    enum Status { Loaded, NoFile, LoadingFailed, NotLoaded };
    Q_ENUM(Status)
private:
    Settings &settings;
    QString errorMessage;
    Q_PROPERTY(QString errorMessage READ getErrorMessage NOTIFY errorMessageChanged)
    Q_PROPERTY(Status status READ getStatus NOTIFY statusChanged)
    Status status = NotLoaded;

public:
    explicit SettingsFileWrapper(Settings &settings) : settings(settings) {}
    [[nodiscard]] Status getStatus() const { return status; }
    void setStatus(Status status);
    [[nodiscard]] QString getErrorMessage() const { return errorMessage; }
    void setErrorMessage(const QString &errorMessage);
    Q_INVOKABLE void loadFile(const QString &filepath);
    Q_INVOKABLE void reload() { loadFile(settings.getJsonSettingsFilePath()); }
    Q_INVOKABLE bool openFileInDefaultEditor();
    Q_INVOKABLE void openFileExplorerAtFile();
signals:
    void statusChanged();
    void errorMessageChanged();
};

#endif // SETTINGSFILEWRAPPER_H
