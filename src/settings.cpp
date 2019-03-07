#include "settings.h"

QFileInfo Settings::localSettingsFile;

Settings::Settings(QObject *parent) : QObject(parent), settings("Turmstraße 1 e.V.","Lichtsteuerung")
{
    if(localSettingsFile.exists()){
        localSettings = QVariant::fromValue(new QSettings(localSettingsFile.filePath(),QSettings::IniFormat));
        localSettings.value<QSettings*>()->setIniCodec("UTF-8");
    }
    Modules::Compiler::compilerCmd = value("compilerCmd",Modules::Compiler::compilerCmd).toString();
    Modules::Compiler::compilerFlags = value("compilerFlags",Modules::Compiler::compilerFlags).toString();
    Modules::Compiler::compilerLibraryFlags = value("compilerLibraryFlags",Modules::Compiler::compilerLibraryFlags).toString();
    Modules::Compiler::includePath = value("includePath",Modules::Compiler::includePath).toString();
}


Settings::~Settings(){
    if(localSettings.isValid()){
        delete localSettings.value<QSettings*>();
    }
}

void Settings::setValue(const QString &key, const QVariant &value){
    if(localSettings.isValid()){
        if(localSettings.value<QSettings*>()->contains(key)){
            localSettings.value<QSettings*>()->setValue(key,value);
        }
    }
    settings.setValue(key,value);
}
QVariant Settings::value(const QString &key, const QVariant &defaultValue) const{
    if(localSettings.isValid()){
        if(localSettings.value<QSettings*>()->contains(key)){
            return localSettings.value<QSettings*>()->value(key,defaultValue);
        }
    }
    return settings.value(key,defaultValue);
}
