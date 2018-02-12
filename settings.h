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
public:
    explicit Settings(QObject *parent = nullptr);    
    void setJsonSettingsFilePath(QString file){if(!QFile::exists(file))return;settings.setValue("jsonSettingsFilePath",file);emit jsonSettingsFilePathChanged();}
    QString getJsonSettingsFilePath()const{return settings.value("jsonSettingsFilePath").toString();}
signals:
    void jsonSettingsFilePathChanged();
public slots:
};

#endif // SETTINGS_H
