#include "settings.h"

Settings::Settings(QObject *parent) : QObject(parent), settings("Turmstraße 1 e.V.","Lichtsteuerung")
{

    Modules::Compiler::compilerCmd = settings.value("compilerCmd",Modules::Compiler::compilerCmd).toString();
    Modules::Compiler::compilerFlags = settings.value("compilerFlags",Modules::Compiler::compilerFlags).toString();
    Modules::Compiler::compilerLibraryFlags = settings.value("compilerLibraryFlags",Modules::Compiler::compilerLibraryFlags).toString();
    Modules::Compiler::includePath = settings.value("includePath",Modules::Compiler::includePath).toString();
}

Settings::Settings(const QFileInfo &settingsFile, QObject *parent) : QObject(parent), settings("Turmstraße 1 e.V.","Lichtsteuerung"), localSettings(QVariant::fromValue(new QSettings(settingsFile.filePath(),QSettings::IniFormat)))
{
    localSettings.value<QSettings*>()->setIniCodec("UTF-8");
    Modules::Compiler::compilerCmd = settings.value("compilerCmd",Modules::Compiler::compilerCmd).toString();
    Modules::Compiler::compilerFlags = settings.value("compilerFlags",Modules::Compiler::compilerFlags).toString();
    Modules::Compiler::compilerLibraryFlags = settings.value("compilerLibraryFlags",Modules::Compiler::compilerLibraryFlags).toString();
    Modules::Compiler::includePath = settings.value("includePath",Modules::Compiler::includePath).toString();
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
