#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QFile>
#include <QDir>
#include "modules/compiler.h"

class Settings : public QObject
{
    Q_OBJECT
    QSettings settings;
    QVariant localSettings; //TODO replace with std::optionl when using c++17
    Q_PROPERTY(QString jsonSettingsFilePath READ getJsonSettingsFilePath WRITE setJsonSettingsFilePath NOTIFY jsonSettingsFilePathChanged)
    Q_PROPERTY(QString driverFilePath READ getDriverFilePath WRITE setDriverFilePath NOTIFY driverFilePathChanged)
    Q_PROPERTY(QString moduleDirPath READ getModuleDirPath WRITE setModuleDirPath NOTIFY moduleDirPathChanged)
    Q_PROPERTY(QString compilerPath READ getCompilerPath WRITE setCompilerPath NOTIFY compilerPathChanged)
    Q_PROPERTY(QString compilerFlags READ getCompilerFlags WRITE setCompilerFlags NOTIFY compilerFlagsChanged)
    Q_PROPERTY(QString compilerLibraryFlags READ getCompilerLibraryFlags WRITE setCompilerLibraryFlags NOTIFY compilerLibraryFlagsChanged)
    Q_PROPERTY(QString includePath READ getIncludePath WRITE setIncludePath NOTIFY includePathChanged)
    Q_PROPERTY(QString audioCaptureFilePath READ getAudioCaptureFilePath WRITE setAudioCaptureFilePath NOTIFY audioCaptureFilePathChanged)
    Q_PROPERTY(unsigned int updatePauseInMs READ getUpdatePauseInMs WRITE setUpdatePauseInMs NOTIFY updatePauseInMsChanged)
    static QFileInfo localSettingsFile;
public:
    /**
     * @brief setLocalSettingFile sets a localSetting file that is loaded and preferred everytime a Settings object is created
     * @param settingFile the filePath to the local setting file in utf8 ini format
     */
    static void setLocalSettingFile(const QFileInfo & settingFile){
        localSettingsFile = settingFile;
    }
protected:
    void setValue(const QString &key, const QVariant &value);
    QVariant value(const QString &key, const QVariant &defaultValue = QVariant())const;
public:
    explicit Settings(QObject *parent = nullptr);
    ~Settings();
    void setJsonSettingsFilePath(QString file){
        if(file == getJsonSettingsFilePath())return;
        if(QFile::exists(file)){
            setValue("jsonSettingsFilePath",file);
            emit jsonSettingsFilePathChanged();
        }
    }
    QString getJsonSettingsFilePath()const{return value("jsonSettingsFilePath").toString();}

    void setDriverFilePath(QString file){
        if(file == getDriverFilePath())return;
        if(QFile::exists(file)){
            setValue("driverFilePath",file);
            emit driverFilePathChanged();
        }
    }
    QString getDriverFilePath()const{return value("driverFilePath").toString();}

    void setAudioCaptureFilePath(QString file){
        if(file == getAudioCaptureFilePath()){
            return;
        }
        if(QFile::exists(file)){
            setValue("audioCaptureFilePath",file);
            emit audioCaptureFilePathChanged();
        }
    }
    QString getAudioCaptureFilePath()const{return value("audioCaptureFilePath").toString();}

    void setUpdatePauseInMs(unsigned int pause){setValue("updatePauseInMs",pause);emit updatePauseInMsChanged();}
    unsigned int getUpdatePauseInMs()const{return value("updatePauseInMs").toUInt();}
    void setModuleDirPath( const QString &_moduleDirPath){
        if(_moduleDirPath == getModuleDirPath())
            return;
        if(!QDir(_moduleDirPath).exists())
            return;
        setValue("moduleDirPath",_moduleDirPath);
        emit moduleDirPathChanged();
    }

    QString getModuleDirPath() const {
        return value("moduleDirPath").toString();
    }

    void setCompilerPath( const QString &_compilerPath){
        if(_compilerPath != Modules::Compiler::compilerCmd){
            setValue("compilerCmd",_compilerPath);
            Modules::Compiler::compilerCmd = _compilerPath;
            emit compilerPathChanged();
        }
    }
    QString getCompilerPath() const {
        return Modules::Compiler::compilerCmd;
    }
    void setIncludePath( const QString &_includePath){
            if(_includePath != Modules::Compiler::includePath){
                    Modules::Compiler::includePath = _includePath;
                    setValue("includePath",_includePath);
                    emit includePathChanged();
            }
    }
    QString getIncludePath() const {
            return Modules::Compiler::includePath;
    }

    void setCompilerFlags( const QString &_compilerFlags){
      if(_compilerFlags != Modules::Compiler::compilerFlags){
          setValue("compilerFlags",_compilerFlags);
        Modules::Compiler::compilerFlags = _compilerFlags;
        emit compilerFlagsChanged();
      }
    }
    QString getCompilerFlags() const {
        return Modules::Compiler::compilerFlags;
    }

    void setCompilerLibraryFlags( const QString &_compilerLibraryFlags){
        if(_compilerLibraryFlags != Modules::Compiler::compilerLibraryFlags){
            setValue("compilerLibraryFlags",_compilerLibraryFlags);
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
    void includePathChanged();
    void audioCaptureFilePathChanged();
public slots:
};

#endif // SETTINGS_H
