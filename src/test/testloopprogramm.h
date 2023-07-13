#ifndef TESTLOOPPROGRAMM_H
#define TESTLOOPPROGRAMM_H

#include "modules/loopprogram.hpp"
#include "modules/program.hpp"
#include "modules/types.h"

#include <qdebug.h>
#include <iostream>

namespace Test {
void testLoopProgramm();

class TestLoopProgramm : public Modules::TypedLoopProgram<Modules::brightness_t> {
protected:
    virtual void loopProgram() override {
        for (unsigned i = 0; i < outputLength; ++i) {
            output[i] = 255;
            // qDebug()<<i;
            wait(50);
        }
    }

public:
    TestLoopProgramm() = default;
    virtual const char *getName() const override { return "TestLoopProgramm"; }
    virtual int getProgrammLengthInMS() override { return outputLength * 50; }
    virtual ~TestLoopProgramm() override = default;
};
} // namespace Test

#endif // TESTLOOPPROGRAMM_H
