#include "settings.h"

Settings::Settings(QObject *parent) : QObject(parent), settings("Turmstraße 1 e.V.","Lichtsteuerung")
{

    Modules::Compiler::compilerCmd = settings.value("compilerCmd",Modules::Compiler::compilerCmd).toString();
    Modules::Compiler::compilerFlags = settings.value("compilerFlags",Modules::Compiler::compilerFlags).toString();
    Modules::Compiler::compilerLibraryFlags = settings.value("compilerLibraryFlags",Modules::Compiler::compilerLibraryFlags).toString();
    Modules::Compiler::includePath = settings.value("includePath",Modules::Compiler::includePath).toString();
}
