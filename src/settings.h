#ifndef SETTINGS_H
#define SETTINGS_H

#include "modules/compiler.h"
#include <QColor>
#include <QDir>
#include <QFile>
#include <QObject>
#include <QSettings>
#include <QSysInfo>
#include <optional>

#define MAKE_STRING(s) #s

#define ThemeColorProperty(type, paramType, name, defaultLight, defaultDark)                                                                                                                                                                                                                               \
private:                                                                                                                                                                                                                                                                                                   \
    Q_PROPERTY(type name READ get##name WRITE set##name NOTIFY name##Changed RESET reset##name)                                                                                                                                                                                                            \
public:                                                                                                                                                                                                                                                                                                    \
    type get##name() {                                                                                                                                                                                                                                                                                     \
        if (getTheme() == 0) {                                                                                                                                                                                                                                                                             \
            if (auto v = value(QStringLiteral(MAKE_STRING(light##name))); !v.isNull()) {                                                                                                                                                                                                                   \
                return v.value<type>();                                                                                                                                                                                                                                                                    \
            }                                                                                                                                                                                                                                                                                              \
            return defaultLight;                                                                                                                                                                                                                                                                           \
        }                                                                                                                                                                                                                                                                                                  \
        if (auto v = value(QStringLiteral(MAKE_STRING(dark##name))); !v.isNull()) {                                                                                                                                                                                                                        \
            return v.value<type>();                                                                                                                                                                                                                                                                        \
        }                                                                                                                                                                                                                                                                                                  \
        return defaultDark;                                                                                                                                                                                                                                                                                \
    }                                                                                                                                                                                                                                                                                                      \
    void set##name(paramType c) {                                                                                                                                                                                                                                                                          \
        if (getTheme() == 0) {                                                                                                                                                                                                                                                                             \
            if (c != get##name()) {                                                                                                                                                                                                                                                                        \
                setValue(QStringLiteral(MAKE_STRING(light##name)), c);                                                                                                                                                                                                                                     \
                emit name##Changed();                                                                                                                                                                                                                                                                      \
            }                                                                                                                                                                                                                                                                                              \
        }                                                                                                                                                                                                                                                                                                  \
        if (c != get##name()) {                                                                                                                                                                                                                                                                            \
            setValue(QStringLiteral(MAKE_STRING(dark##name)), c);                                                                                                                                                                                                                                          \
            emit name##Changed();                                                                                                                                                                                                                                                                          \
        }                                                                                                                                                                                                                                                                                                  \
    }                                                                                                                                                                                                                                                                                                      \
    void reset##name() { set##name(getTheme() == 0 ? (defaultLight) : (defaultDark)); }                                                                                                                                                                                                                    \
Q_SIGNALS:                                                                                                                                                                                                                                                                                                 \
    void name##Changed();

class Settings : public QObject {
    Q_OBJECT
    QSettings settings;
    std::optional<QSettings> localSettings;
    QString jsonSettingsFileSavePath;
    bool loadFromJsonSettingsFilePath = false;
    Q_PROPERTY(QString jsonSettingsFilePath READ getJsonSettingsFilePath NOTIFY jsonSettingsFilePathChanged)
    Q_PROPERTY(QString driverFilePath READ getDriverFilePath WRITE setDriverFilePath NOTIFY driverFilePathChanged)
    Q_PROPERTY(QString moduleDirPath READ getModuleDirPath WRITE setModuleDirPath NOTIFY moduleDirPathChanged)
    Q_PROPERTY(QString compilerPath READ getCompilerPath WRITE setCompilerPath NOTIFY compilerPathChanged)
    Q_PROPERTY(QString compilerFlags READ getCompilerFlags WRITE setCompilerFlags NOTIFY compilerFlagsChanged)
    Q_PROPERTY(QString compilerLibraryFlags READ getCompilerLibraryFlags WRITE setCompilerLibraryFlags NOTIFY compilerLibraryFlagsChanged)
    Q_PROPERTY(QString includePath READ getIncludePath WRITE setIncludePath NOTIFY includePathChanged)
    Q_PROPERTY(QString computerName READ getComputerName WRITE setComputerName NOTIFY computerNameChanged)
    Q_PROPERTY(bool remoteVolumeControl READ remoteVolumeControl WRITE remoteVolumeControl NOTIFY remoteVolumeControlChanged)
    Q_PROPERTY(int theme READ getTheme WRITE setTheme NOTIFY themeChanged)
    Q_PROPERTY(unsigned int updatePauseInMs READ getUpdatePauseInMs WRITE setUpdatePauseInMs NOTIFY updatePauseInMsChanged)
    static inline QFileInfo localSettingsFile;

public:
    static constexpr auto OrganisationName = "Turmstraße 1 e.V.";
    static constexpr auto ApplicationName = "Lichtsteuerung";
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

    /**
     * @brief setJsonSettingsFilePath only sets the settings file path to the given path. For an advanced
     *        variant see setJsonSettingsFilePath(const QString&, bool)
     * @param file the new filepath
     */
    void setJsonSettingsFilePath(const QString &file);

    /**
     * @brief setJsonSettingsFilePath sets the settings file path and handles the new file. If loadFromPath is
     *        true, shouldLoadFromSettingsPath() will return false and getJsonSettingsFileSavePath() will return
     *        the old path. If loadFromPath is false, shouldLoadFromSettingsPath() will return false. The signal
     *        saveAs(QString) will be emitted. getJsonSettingsFileSavePath() will return the new path.
     *
     * @param file the new settings file path
     * @param loadFromPath true if the settings should be loaded from the new file, false if the settings should
     *        be written to the new path
     * @return false, if file == getJsonSettingsFilePath() or loadFromPath is true and the file does not exists.
     *         True otherwise
     */
    Q_INVOKABLE bool setJsonSettingsFilePath(const QString &file, bool loadFromPath);
    [[nodiscard]] QString getJsonSettingsFilePath() const { return value(QStringLiteral("jsonSettingsFilePath")).toString(); }
    /**
     * @brief getJsonSettingsFileSavePath returns the path, to which the settings file should be written
     * @return the path where to save the settings file
     */
    [[nodiscard]] QString getJsonSettingsFileSavePath() const { return jsonSettingsFileSavePath; }
    /**
     * @brief shouldLoadFromSettingsPath indicates, if the settings should be loaded and not saved to the json settings path
     * @return true if the settings should be loaded from the path, false if the settings should be saved to the path
     */
    [[nodiscard]] bool shouldLoadFromSettingsPath() const {
        return loadFromJsonSettingsFilePath;
    }

    void setDriverFilePath(const QString& file){
        if(file == getDriverFilePath()) {
            return;
        }
        if(QFile::exists(file)){
            setValue(QStringLiteral("driverFilePath"),file);
            emit driverFilePathChanged();
        }
    }
    QString getDriverFilePath()const{return value(QStringLiteral("driverFilePath")).toString();}

    void setUpdatePauseInMs(unsigned int pause){setValue(QStringLiteral("updatePauseInMs"),pause);emit updatePauseInMsChanged();}
    unsigned int getUpdatePauseInMs()const{return value(QStringLiteral("updatePauseInMs")).toUInt();}
    void setModuleDirPath( const QString &_moduleDirPath){
        if(_moduleDirPath == getModuleDirPath()) {
            return;
        }
        if(!QDir(_moduleDirPath).exists()) {
            return;
        }
        setValue(QStringLiteral("moduleDirPath"),_moduleDirPath);
        emit moduleDirPathChanged();
    }

    QString getModuleDirPath() const {
        return value(QStringLiteral("moduleDirPath")).toString();
    }

    void setCompilerPath( const QString &_compilerPath){
        if(_compilerPath != Modules::Compiler::compilerCmd){
            setValue(QStringLiteral("compilerCmd"),_compilerPath);
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
                    setValue(QStringLiteral("includePath"),_includePath);
                    emit includePathChanged();
            }
    }
    QString getIncludePath() const {
            return Modules::Compiler::includePath;
    }

    void setCompilerFlags( const QString &_compilerFlags){
      if(_compilerFlags != Modules::Compiler::compilerFlags){
          setValue(QStringLiteral("compilerFlags"),_compilerFlags);
        Modules::Compiler::compilerFlags = _compilerFlags;
        emit compilerFlagsChanged();
      }
    }
    QString getCompilerFlags() const {
        return Modules::Compiler::compilerFlags;
    }

    void setCompilerLibraryFlags( const QString &_compilerLibraryFlags){
        if(_compilerLibraryFlags != Modules::Compiler::compilerLibraryFlags){
            setValue(QStringLiteral("compilerLibraryFlags"),_compilerLibraryFlags);
            Modules::Compiler::compilerLibraryFlags = _compilerLibraryFlags; emit compilerLibraryFlagsChanged();
        }
    }
    QString getCompilerLibraryFlags() const { return Modules::Compiler::compilerLibraryFlags; }

    void setComputerName(const QString &name);
    [[nodiscard]] QString getComputerName() const { return value(QStringLiteral("ComputerName"), QSysInfo::machineHostName()).toString(); }

    void remoteVolumeControl(bool enable);
    [[nodiscard]] bool remoteVolumeControl() const { return value(QStringLiteral("remoteVolumeControl"), false).toBool(); }

    void setTheme(int theme) {
        if (theme != getTheme()) {
            setValue(QStringLiteral("theme"), theme);
            emit themeChanged();
            emit foregroundColorChanged();
            emit foregroundMaterialChanged();
            emit foregroundShadeChanged();
            emit backgroundColorChanged();
            emit backgroundMaterialChanged();
            emit backgroundShadeChanged();
            emit accentColorChanged();
            emit accentMaterialChanged();
            emit accentShadeChanged();
            emit popupBackgroundEffectChanged();
            emit popupBackgroundEffectIntensityChanged();
        }
    }
    int getTheme() { return value(QStringLiteral("theme")).toInt(); }

    ThemeColorProperty(QColor, const QColor &, foregroundColor, QColor(Qt::black), QColor(0xfa, 0xfa, 0xfa))
    ThemeColorProperty(int, int, foregroundMaterial, -1, -1)
    ThemeColorProperty(int, int, foregroundShade, -1, -1)
    ThemeColorProperty(QColor, const QColor &, backgroundColor, QColor(0xfa, 0xfa, 0xfa), QColor(0x30, 0x30, 0x30))
    ThemeColorProperty(int, int, backgroundMaterial, -1, -1)
    ThemeColorProperty(int, int, backgroundShade, -1, -1)
    ThemeColorProperty(QColor, const QColor &, accentColor, QColor(0xE9, 0x1E, 0x63), QColor(0xF4, 0x8F, 0xB1))
    ThemeColorProperty(int, int, accentMaterial, 1, 1)
    ThemeColorProperty(int, int, accentShade, -1, -1)
public:
    enum PopupBackgroundEffect { Dim, Blur };
    Q_ENUM(PopupBackgroundEffect)
    enum PopupBackgroundEffectIntensity { Weak, Normal, Strong };
    Q_ENUM(PopupBackgroundEffectIntensity)
    ThemeColorProperty(PopupBackgroundEffect, PopupBackgroundEffect, popupBackgroundEffect, Dim, Blur)
    ThemeColorProperty(PopupBackgroundEffectIntensity, PopupBackgroundEffectIntensity, popupBackgroundEffectIntensity, Normal, Normal)

signals:
    void jsonSettingsFilePathChanged();
    void driverFilePathChanged();
    void updatePauseInMsChanged();
    void moduleDirPathChanged();
    void compilerPathChanged();
    void compilerFlagsChanged();
    void compilerLibraryFlagsChanged();
    void includePathChanged();
    void themeChanged();
    void saveAs(QString path);
    void computerNameChanged();
    void remoteVolumeControlChanged();
public slots:
};

#endif // SETTINGS_H
