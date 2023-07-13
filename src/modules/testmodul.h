#ifndef TESTMODUL_H
#define TESTMODUL_H

#define HAVE_FILTER
#include "filter.hpp"
#include "module.h"

unsigned int getNumberOfFilter() {
    return 1;
}
char const *getNameOfFilter(unsigned int index) {
    switch (index) {
    case 0: return "Blur";
    default: return "Invalid Index";
    }
}
char const *getDescriptionOfFilter(unsigned int index) {
    switch (index) {
    case 0: return "Blurs the Input data";
    default: return "Invalid Index";
    }
}

using namespace Modules;

class Blur : public TypedFilter<brightness_t, brightness_t> {
protected:
    virtual unsigned int computeOutputLength(unsigned int inputLength) override { return inputLength; }
    NumericProperty<int> *blurRangeProp;

public:
    Blur() {
        properties.emplace_back(1, 15, 3);
        blurRangeProp = properties[0].asInt();
    }
    virtual void filter() override {
        const int blurRange = blurRangeProp->getValue();
        for (int i = 0; i < getInputLength(); ++i) {
            int sum = 0;
            for (int j = i - blurRange; j <= i + blurRange; ++j) {
                if (j < 0) {
                    sum += input[0];
                } else if (j >= inputLength) {
                    sum += input[inputLength - 1];
                } else {
                    sum += input[j];
                }
            }
            output[i] = brightness_t(sum / (2.f * blurRange + 1));
        }
    }
};

Modules::Filter *createFilter(unsigned int index) {
    switch (index) {
    case 0: return new Blur();
    default: return nullptr;
    }
}

#endif // TESTMODUL_H
