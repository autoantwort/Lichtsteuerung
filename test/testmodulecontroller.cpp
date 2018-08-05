#include "testmodulecontroller.h"
#include "programms/modulemanager.h"
#include <chrono>


void Test::createProgrammBlockAndTestRunIt(){

    using namespace Modules;
    using namespace std::chrono;
    ModuleManager::singletone()->controller().start();
    auto mm = ModuleManager::singletone();
    for(const auto & c : mm->getConsumerModules()){
        qDebug() << QString::fromStdString(c.name());
    }for(const auto & c : mm->getFilterModules()){
        qDebug() << QString::fromStdString(c.name());
    }for(const auto & c : mm->getProgrammModules()){
        qDebug() << QString::fromStdString(c.name());
    }
    auto con = std::make_shared<ProgramBlock>();
    auto lauflicht = mm->createProgramm(/*"Lauflicht"*/"TestProgramm");
    auto blur = mm->createFilter("GaussianBlur");
    auto rgbstrip = mm->createConsumer("WINBrightnessConsumer");
    if(!lauflicht || !blur || !rgbstrip){
        qDebug() << "Fehler beim erstellen" << lauflicht.get() << ' '<<blur.get() << ' ' << rgbstrip.get();
        return;
    }
    const auto LENGTH = 50;
    lauflicht->getProperties()[0]->asNumeric<int>()->setValue(4);
    lauflicht->getProperties()[1]->asNumeric<int>()->setValue(255);
    lauflicht->getProperties()[2]->asNumeric<int>()->setValue(0);
    lauflicht->setOutputLength(LENGTH);
    blur->setInputLength(LENGTH);
    blur->getProperties()[0]->asNumeric<int>()->setValue(5);
    blur->getProperties()[1]->asNumeric<double>()->setValue(1);
    rgbstrip->setInputLength(LENGTH);
    rgbstrip->getProperties()[0]->asNumeric<int>()->setValue(5);
    rgbstrip->getProperties()[1]->asNumeric<int>()->setValue(2);
    Modules::detail::Connection first(blur);
    first.addTarget(LENGTH,lauflicht.get(),0);
    Modules::detail::Connection second(rgbstrip);
    second.addTarget(LENGTH,blur.get(),0);
    con->addProgramm(lauflicht);
    con->addFilter(first,0);
    con->addConsumer(second);
    mm->controller().runProgramm(con);
    mm->controller().start();
}
