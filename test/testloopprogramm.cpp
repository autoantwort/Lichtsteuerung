#include "testloopprogramm.h"
#include <qdebug.h>
#include <chrono>
#include "programms/boostloopprogramcontextswitcher.h"

namespace Test {

void testLoopProgramm(){
    auto cs = std::make_unique<Modules::BoostLoopProgramContextSwitcher>();
    TestLoopProgramm p;
    p.setContextSwitcher(std::move(cs));
    p.setOutputLength(10000);
    p.start();
    auto t1 = std::chrono::high_resolution_clock::now();
    while (!p.doStep(2).finished) {
        //qDebug() << "wait";
    }
    auto tdiff = std::chrono::high_resolution_clock::now()-t1;
    qDebug()<<(std::chrono::duration_cast<std::chrono::milliseconds>(tdiff)).count() <<" finished\n";
}

}
