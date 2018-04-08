#include "testloopprogramm.h"
#include <qdebug.h>
#include <chrono>


void test(){
    TestLoopProgramm p;
    p.setOutputLength(10000);
    p.start();
    auto t1 = std::chrono::high_resolution_clock::now();
    while (!p.doStep(2)) {

    }
    auto tdiff = std::chrono::high_resolution_clock::now()-t1;
    qDebug()<<(tdiff).count() <<"finished5\n";
}
