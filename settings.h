#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QFile>

class Settings : public QObject
{
    Q_OBJECT
    QSettings settings;
    Q_PROPERTY(QString jsonSettingsFilePath READ getJsonSettingsFilePath WRITE setJsonSettingsFilePath NOTIFY jsonSettingsFilePathChanged)
    Q_PROPERTY(QString driverFilePath READ getDriverFilePath WRITE setDriverFilePath NOTIFY driverFilePathChanged)
    Q_PROPERTY(QString audioCaptureFilePath READ getAudioCaptureFilePath WRITE setAudioCaptureFilePath NOTIFY audioCaptureFilePathChanged)
    Q_PROPERTY(unsigned int updatePauseInMs READ getUpdatePauseInMs WRITE setUpdatePauseInMs NOTIFY updatePauseInMsChanged)
public:
    explicit Settings(QObject *parent = nullptr);
    void setJsonSettingsFilePath(QString file){if(!QFile::exists(file))return;settings.setValue("jsonSettingsFilePath",file);emit jsonSettingsFilePathChanged();}
    QString getJsonSettingsFilePath()const{return settings.value("jsonSettingsFilePath").toString();}

    void setDriverFilePath(QString file){if(!QFile::exists(file))return;settings.setValue("driverFilePath",file);emit driverFilePathChanged();}
    QString getDriverFilePath()const{return settings.value("driverFilePath").toString();}

    void setAudioCaptureFilePath(QString file){if(!QFile::exists(file))return;settings.setValue("audioCaptureFilePath",file);emit audioCaptureFilePathChanged();}
    QString getAudioCaptureFilePath()const{return settings.value("audioCaptureFilePath").toString();}

    void setUpdatePauseInMs(unsigned int pause){settings.setValue("updatePauseInMs",pause);emit updatePauseInMsChanged();}
    unsigned int getUpdatePauseInMs()const{return settings.value("updatePauseInMs").toUInt();}
signals:
    void jsonSettingsFilePathChanged();
    void driverFilePathChanged();
    void updatePauseInMsChanged();
    void audioCaptureFilePathChanged();
public slots:
};

#endif // SETTINGS_H
