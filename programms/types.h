#ifndef TYPES_H
#define TYPES_H

#include "property.h"
#include <vector>

enum ValueType{Brightness, RGB};

typedef int time_diff_t;

typedef unsigned char brightness_t;
static_assert (sizeof (brightness_t)==1, "size of unsigned char is not 1");

struct rgb_t{
    union{
        brightness_t r;
        brightness_t red;
    };
    union{
        brightness_t g;
        brightness_t green;
    };
    union{
        brightness_t b;
        brightness_t blue;
    };
};

class PropertyBase{
protected:
    std::vector<Property> properties;
public:
    const std::vector<Property>& getProperties()const{return properties;}
};

#endif // TYPES_H
