#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QFile>
#include "programms/compiler.h"

class Settings : public QObject
{
    Q_OBJECT
    QSettings settings;
    Q_PROPERTY(QString jsonSettingsFilePath READ getJsonSettingsFilePath WRITE setJsonSettingsFilePath NOTIFY jsonSettingsFilePathChanged)
    Q_PROPERTY(QString driverFilePath READ getDriverFilePath WRITE setDriverFilePath NOTIFY driverFilePathChanged)
    Q_PROPERTY(QString moduleDirPath READ getModuleDirPath WRITE setModuleDirPath NOTIFY moduleDirPathChanged)
    Q_PROPERTY(QString compilerPath READ getCompilerPath WRITE setCompilerPath NOTIFY compilerPathChanged)
    Q_PROPERTY(QString compilerFlags READ getCompilerFlags WRITE setCompilerFlags NOTIFY compilerFlagsChanged)
    Q_PROPERTY(QString compilerLibraryFlags READ getCompilerLibraryFlags WRITE setCompilerLibraryFlags NOTIFY compilerLibraryFlagsChanged)
    Q_PROPERTY(unsigned int updatePauseInMs READ getUpdatePauseInMs WRITE setUpdatePauseInMs NOTIFY updatePauseInMsChanged)
public:
    explicit Settings(QObject *parent = nullptr);
    void setJsonSettingsFilePath(QString file){if(!QFile::exists(file))return;settings.setValue("jsonSettingsFilePath",file);emit jsonSettingsFilePathChanged();}
    QString getJsonSettingsFilePath()const{return settings.value("jsonSettingsFilePath").toString();}
    void setDriverFilePath(QString file){if(!QFile::exists(file))return;settings.setValue("driverFilePath",file);emit driverFilePathChanged();}
    QString getDriverFilePath()const{return settings.value("driverFilePath").toString();}
    void setUpdatePauseInMs(unsigned int pause){settings.setValue("updatePauseInMs",pause);emit updatePauseInMsChanged();}
    unsigned int getUpdatePauseInMs()const{return settings.value("updatePauseInMs").toUInt();}
    void setModuleDirPath( const QString _moduleDirPath){
        if(!QFile::exists(_moduleDirPath))return;
            settings.setValue("moduleDirPath",_moduleDirPath);
            emit moduleDirPathChanged();
    }

    QString getModuleDirPath() const {
        return settings.value("moduleDirPath").toString();
    }

    void setCompilerPath( const QString _compilerPath){
        if(_compilerPath != Modules::Compiler::compilerCmd){
            settings.setValue("compilerCmd",_compilerPath);
            Modules::Compiler::compilerCmd = _compilerPath;
            emit compilerPathChanged();
        }
    }
    QString getCompilerPath() const {
        return Modules::Compiler::compilerCmd;
    }

    void setCompilerFlags( const QString _compilerFlags){
      if(_compilerFlags != Modules::Compiler::compilerFlags){
          settings.setValue("compilerFlags",_compilerFlags);
        Modules::Compiler::compilerFlags = _compilerFlags;
        emit compilerFlagsChanged();
      }
    }
    QString getCompilerFlags() const {
        return Modules::Compiler::compilerFlags;
    }

    void setCompilerLibraryFlags( const QString _compilerLibraryFlags){
        if(_compilerLibraryFlags != Modules::Compiler::compilerLibraryFlags){
            settings.setValue("compilerLibraryFlags",_compilerLibraryFlags);
            Modules::Compiler::compilerLibraryFlags = _compilerLibraryFlags; emit compilerLibraryFlagsChanged();
        }
    }
    QString getCompilerLibraryFlags() const {
        return Modules::Compiler::compilerLibraryFlags;
    }
signals:
    void jsonSettingsFilePathChanged();
    void driverFilePathChanged();
    void updatePauseInMsChanged();
    void moduleDirPathChanged();
    void compilerPathChanged();
    void compilerFlagsChanged();
    void compilerLibraryFlagsChanged();
public slots:
};

#endif // SETTINGS_H
