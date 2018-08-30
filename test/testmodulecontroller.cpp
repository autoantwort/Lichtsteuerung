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
    auto toRGB = mm->createFilter("toRGB");
    auto rgbstrip = mm->createConsumer("WINBRGBConsumer");
    if(!lauflicht || !blur || !toRGB|| !rgbstrip){
        qDebug() << "Fehler beim erstellen" << lauflicht.get() << ' '<<blur.get() << ' ' <<toRGB.get() << ' ' << rgbstrip.get();
        return;
    }
    const auto LENGTH = 50;
    lauflicht->getProperties()[0]->asNumeric<int>()->setValue(20);
    lauflicht->getProperties()[1]->asNumeric<int>()->setValue(255);
    lauflicht->getProperties()[2]->asNumeric<int>()->setValue(0);
    lauflicht->setOutputLength(LENGTH);
    blur->setInputLength(LENGTH);
    blur->getProperties()[0]->asNumeric<int>()->setValue(50);
    blur->getProperties()[1]->asNumeric<double>()->setValue(5);
    toRGB->setInputLength(LENGTH);
    toRGB->getProperties()[0]->asNumeric<int>()->setValue(2);
    rgbstrip->setInputLength(LENGTH);
    rgbstrip->getProperties()[0]->asNumeric<int>()->setValue(4);
    rgbstrip->getProperties()[1]->asNumeric<int>()->setValue(2);
    Modules::detail::Connection first(blur);
    first.addTarget(LENGTH,lauflicht.get(),0);
    Modules::detail::Connection second(toRGB);
    second.addTarget(LENGTH,blur.get(),0);
    Modules::detail::Connection third(rgbstrip);
    third.addTarget(LENGTH,toRGB.get(),0);
    con->addProgramm(lauflicht);
    con->addFilter(first,0);
    con->addFilter(second,1);
    con->addConsumer(third);
    mm->controller().runProgramm(con);
    mm->controller().start();
    ProgramBlockManager::model.push_back(con);
}
