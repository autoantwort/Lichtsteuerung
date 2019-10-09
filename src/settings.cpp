#include "settings.h"

Settings::Settings(QObject *parent) : QObject(parent), settings(QStringLiteral("Turmstraße 1 e.V."),QStringLiteral("Lichtsteuerung"))
{
    if(localSettingsFile.exists()){
        localSettings.emplace(localSettingsFile.filePath(),QSettings::IniFormat);
        localSettings->setIniCodec("UTF-8");
    }
    Modules::Compiler::compilerCmd = value(QStringLiteral("compilerCmd"),Modules::Compiler::compilerCmd).toString();
    Modules::Compiler::compilerFlags = value(QStringLiteral("compilerFlags"),Modules::Compiler::compilerFlags).toString();
    Modules::Compiler::compilerLibraryFlags = value(QStringLiteral("compilerLibraryFlags"),Modules::Compiler::compilerLibraryFlags).toString();
    Modules::Compiler::includePath = value(QStringLiteral("includePath"),Modules::Compiler::includePath).toString();
    jsonSettingsFileSavePath = getJsonSettingsFilePath();
}

void Settings::setValue(const QString &key, const QVariant &value){
    if(localSettings){
        if(localSettings->contains(key)){
            localSettings->setValue(key,value);
        }
    }
    settings.setValue(key,value);
}
QVariant Settings::value(const QString &key, const QVariant &defaultValue) const{
    if(localSettings){
        if(localSettings->contains(key)){
            return localSettings->value(key,defaultValue);
        }
    }
    return settings.value(key,defaultValue);
}
