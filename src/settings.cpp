#include "settings.h"

Settings::Settings(QObject *parent) : QObject(parent), settings(OrganisationName, ApplicationName) {
    if (localSettingsFile.exists()) {
        localSettings.emplace(localSettingsFile.filePath(), QSettings::IniFormat);
    }
    Modules::Compiler::compilerCmd = value(QStringLiteral("compilerCmd"), Modules::Compiler::compilerCmd).toString();
    Modules::Compiler::compilerFlags = value(QStringLiteral("compilerFlags"), Modules::Compiler::compilerFlags).toString();
    Modules::Compiler::compilerLibraryFlags = value(QStringLiteral("compilerLibraryFlags"), Modules::Compiler::compilerLibraryFlags).toString();
    Modules::Compiler::includePath = value(QStringLiteral("includePath"), Modules::Compiler::includePath).toString();
    jsonSettingsFileSavePath = getJsonSettingsFilePath();
    lastSettings = this;
}

void Settings::setJsonSettingsFilePath(const QString &file) {
    if (file == getJsonSettingsFilePath()) {
        setValue(QStringLiteral("jsonSettingsFilePath"), file);
        emit jsonSettingsFilePathChanged();
    }
}

bool Settings::setJsonSettingsFilePath(const QString &file, bool loadFromPath) {
    if (file == getJsonSettingsFilePath()) {
        return false;
    }
    if (loadFromPath) {
        if (!QFile::exists(file)) {
            return false;
        }
        // it was load/save path, but we dident loaded and now have a new load path
        this->loadFromJsonSettingsFilePath = true;
        setValue(QStringLiteral("jsonSettingsFilePath"), file);
        emit jsonSettingsFilePathChanged();

    } else {
        // it was load or save path, but we dident loaded and now want to save
        this->loadFromJsonSettingsFilePath = false;
        this->jsonSettingsFileSavePath = file;
        setValue(QStringLiteral("jsonSettingsFilePath"), file);
        emit jsonSettingsFilePathChanged();
        emit saveAs(file);
    }
    return true;
}

void Settings::setComputerName(const QString &name) {
    if (name != getComputerName()) {
        setValue(QStringLiteral("ComputerName"), name);
        emit computerNameChanged();
    }
}

void Settings::remoteVolumeControl(bool enable) {
    if (enable != remoteVolumeControl()) {
        setValue(QStringLiteral("remoteVolumeControl"), enable);
        emit remoteVolumeControlChanged();
    }
}

void Settings::setStartupVolumeEnabled(bool enabled) {
    if (enabled != isStartupVolumeEnabled()) {
        setValue(QStringLiteral("isStartupVolumeEnabled"), enabled);
        emit isStartupVolumeEnabledChanged();
    }
}

void Settings::setStartupVolume(double volume) {
    if (volume != getStartupVolume()) {
        setValue(QStringLiteral("StartupVolume"), volume);
        emit startupVolumeChanged();
    }
}

void Settings::setValue(const QString &key, const QVariant &value) {
    if (localSettings) {
        if (localSettings->contains(key)) {
            localSettings->setValue(key, value);
        }
    }
    settings.setValue(key, value);
}
QVariant Settings::value(const QString &key, const QVariant &defaultValue) const {
    if (localSettings) {
        if (localSettings->contains(key)) {
            return localSettings->value(key, defaultValue);
        }
    }
    return settings.value(key, defaultValue);
}
