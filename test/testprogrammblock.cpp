#include "testprogrammblock.h"
#include <programms/programblock.h>

namespace Test {

TestProgrammBlock::TestProgrammBlock()
{

}

void TestProgrammBlock::test(){
    using namespace Modules;
    using namespace std;
    using namespace std::chrono_literals;
    Controller controller;
    auto pb = std::make_shared<ProgramBlock>();
    auto prog = std::make_shared<TestProgramm>();
    auto cons = std::make_shared<ControlConsumer>();
    auto filt = std::make_shared<TestFilter>();
    prog->setOutputLength(10);
    cons->setInputLength(10);
    filt->setInputLength(10);
    detail::Connection c(filt);
    c.addTarget(10,prog.get(),0);
    pb->addProgramm(prog);
    pb->addFilter(c,0);
    detail::Connection l(cons);
    l.addTarget(10,filt.get(),0);
    pb->addConsumer(l);

    controller.start();
    controller.runProgramm(pb);
    this_thread::sleep_for(20ms);
    controller.stop();

}

}
