#ifndef TESTMODULSYSTEM_H
#define TESTMODULSYSTEM_H
#include <programms/modulemanager.h>
#include <programms/compiler.h>
#include <QDebug>
#include <iostream>
#include <chrono>

namespace Test{


    using namespace Modules;
    using namespace std::chrono;
    class TestModulSystem
    {
    public:
        TestModulSystem(){
            Compiler::setCompilerCommand("g++-7");
        }
        void runTest(){
             auto t1 = high_resolution_clock::now();
            Compiler::compileToLibrary(QFile("/Users/leanderSchulten/Lichtsteuerung/programms/testmodul.cpp"),"/Users/leanderSchulten/Lichtsteuerung/programms/testmodul.so");
            auto t2 = high_resolution_clock::now();
            ModuleManager::singletone()->loadModule("/Users/leanderSchulten/Lichtsteuerung/programms/testmodul.so");
            auto t3 = high_resolution_clock::now();
            for(const auto& i:ModuleManager::singletone()->getFilterModules()){
                std::cout <<i.name<< " , " << i.description;
            }
            Filter * blurFilter = ModuleManager::singletone()->createFilter("Blur");
            qDebug()<<blurFilter;
            delete blurFilter;
            std::cout << "compile time : " << duration_cast<milliseconds>(t2-t1).count()<< " load time : "<< duration_cast<milliseconds>(t3-t2).count();
        }
    };
}

#endif // TESTMODULSYSTEM_H
