#include "testloopprogramm.h"


void test(){
    TestLoopProgramm p;
    p.setOutputLength(10);
    p.start();
    while (!p.doStep(10)) {

    }
    std::cout<<"finished5\n";
}
