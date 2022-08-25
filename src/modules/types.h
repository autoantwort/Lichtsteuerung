#ifndef TYPES_H
#define TYPES_H

#include "property.hpp"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace Modules {

enum ValueType { Brightness, RGB };

using time_diff_t = int;

using brightness_t = uint8_t;
static_assert(sizeof(brightness_t) == 1, "size of unsigned char is not 1");

struct hsl_t;
struct hsv_t;

struct rgb_t {
    union {
        struct {
            union {
                brightness_t r;
                brightness_t red;
            };
            union {
                brightness_t g;
                brightness_t green;
            };
            union {
                brightness_t b;
                brightness_t blue;
            };
        };
        brightness_t rgb[3];
    };
    rgb_t(brightness_t r = 0, brightness_t g = 0, brightness_t b = 0) : r(r), g(g), b(b) {}
    rgb_t(int r, int g, int b) : r(std::clamp(r, 0, 255)), g(std::clamp(g, 0, 255)), b(std::clamp(b, 0, 255)) {}
    rgb_t(Modules::hsl_t hsl);
    rgb_t(Modules::hsv_t hsv);
    hsl_t toHSL() const;
    hsv_t toHSV() const;
    rgb_t &operator*(brightness_t b) {
        this->r *= b / 255.f;
        this->g *= b / 255.f;
        this->b *= b / 255.f;
        return *this;
    }
    bool operator==(rgb_t other) const { return r == other.r && g == other.g && b == other.b; }
    bool operator!=(rgb_t other) const { return !(*this == other); }
};
static_assert(sizeof(rgb_t) == 3, "size of rgb_t is not 3");

inline std::ostream &operator<<(std::ostream &o, const rgb_t rgb) {
    o << "rgb{red=" << static_cast<int>(rgb.r) << ",green=" << static_cast<int>(rgb.g) << ",blue=" << static_cast<int>(rgb.b) << "}";
    return o;
}

struct hsl_t {
    union {
        float h;
        float hue;
    };
    union {
        float s;
        float saturation;
    };
    union {
        float l;
        float lightness;
    };
    hsl_t(float hue, float saturation = 1.f, float lightness = .5f) : h(hue), s(saturation), l(lightness) {}
    hsl_t() : h(0), s(1.f), l(0.f) {}
    hsl_t(const rgb_t rgb);
    hsl_t(const hsv_t rgb);
    bool isValid() const { return h >= 0 && h <= 360 && s >= 0 && s <= 1 && l >= 0 && l <= 1; }
    void clampHueToRange() {
        hue = std::fmod(hue, 360.f);
        if (hue < 0) {
            hue += 360;
        }
    }
    void clampToRanges() {
        s = std::clamp(s, 0.f, 1.f);
        l = std::clamp(l, 0.f, 1.f);
        clampHueToRange();
    }
    rgb_t toRGB() const { return rgb_t(*this); }
    hsv_t toHSV() const;
};

inline std::ostream &operator<<(std::ostream &o, const hsl_t hsl) {
    o << "hsl{hue=" << hsl.hue << ",saturation=" << hsl.saturation << ",lightness=" << hsl.lightness << "}";
    return o;
}

inline hsl_t::hsl_t(const rgb_t rgb) {
    // see https://de.wikipedia.org/wiki/HSV-Farbraum#Umrechnung_RGB_in_HSV/HSL
    const auto max = std::max(rgb.r, std::max(rgb.g, rgb.b));
    const auto min = std::min(rgb.r, std::min(rgb.g, rgb.b));
    const auto c = max - min;
    if (c == 0) {
        hue = 0;
    } else {
        const auto cf = c / 255.f;
        if (max == rgb.r) {
            const float diff = (rgb.g - rgb.b) / cf;
            hue = diff / c;
        } else if (max == rgb.g) {
            const float diff = (rgb.b - rgb.r) / cf;
            hue = 2 + diff / c;
        } else if (max == rgb.b) {
            const float diff = (rgb.r - rgb.g) / cf;
            hue = 4 + diff / c;
        }
        hue *= 60.f;
        if (hue < 0) {
            hue *= 360.f;
        }
    }
    if (min == 255 || max == 0) {
        s = 0;
    } else {
        const float m = (max - min) / 255.f;
        const float p = (max + min) / 255.f;
        s = m / (1 - std::abs(p - 1));
    }
    l = (max + min) / (2 * 255.f);
}

inline hsl_t rgb_t::toHSL() const {
    return hsl_t(*this);
}

inline rgb_t::rgb_t(const hsl_t hsl) {
    // see https://en.wikipedia.org/wiki/HSL_and_HSV#HSL_to_RGB_alternative
    const auto kf = [&](const auto n) { return std::fmod(n + hsl.h / 30, 12.f); };
    const auto a = [&]() { return hsl.s * std::min(hsl.l, 1 - hsl.l); };
    const auto f = [&](const auto n) {
        const auto k = kf(n);
        return hsl.l - a() * std::max(std::min(1.f, std::min(k - 3, 9 - k)), -1.f);
    };
    r = static_cast<brightness_t>(f(0) * 255.f);
    g = static_cast<brightness_t>(f(8) * 255.f);
    b = static_cast<brightness_t>(f(4) * 255.f);
}

struct hsv_t {
    union {
        float h;
        float hue;
    };
    union {
        float s;
        float saturation;
    };
    union {
        float v;
        float value;
    };
    hsv_t(float hue, float saturation = 1.f, float value = 1.f) : h(hue), s(saturation), v(value) {}
    hsv_t() : h(0), s(1.f), v(0.f) {}
    hsv_t(const rgb_t rgb);
    hsv_t(const hsl_t hsl);
    bool isValid() const { return h >= 0 && h <= 360 && s >= 0 && s <= 1 && v >= 0 && v <= 1; }
    void clampHueToRange() {
        hue = std::fmod(hue, 360.f);
        if (hue < 0) {
            hue += 360;
        }
    }
    void clampToRanges() {
        s = std::clamp(s, 0.f, 1.f);
        v = std::clamp(v, 0.f, 1.f);
        clampHueToRange();
    }
    rgb_t toRGB() const { return *this; }
    hsl_t toHSL() const { return *this; }
};

inline std::ostream &operator<<(std::ostream &o, const hsv_t hsv) {
    o << "hsv{hue=" << hsv.hue << ",saturation=" << hsv.saturation << ",value=" << hsv.value << "}";
    return o;
}

inline hsv_t::hsv_t(const rgb_t rgb) {
    // see https://de.wikipedia.org/wiki/HSV-Farbraum#Umrechnung_RGB_in_HSV/HSL
    const auto max = std::max(rgb.r, std::max(rgb.g, rgb.b));
    const auto min = std::min(rgb.r, std::min(rgb.g, rgb.b));
    const auto c = max - min;
    if (c == 0) {
        hue = 0;
    } else {
        const auto cf = c / 255.f;
        if (max == rgb.r) {
            const float diff = (rgb.g - rgb.b) / cf;
            hue = diff / c;
        } else if (max == rgb.g) {
            const float diff = (rgb.b - rgb.r) / cf;
            hue = 2 + diff / c;
        } else if (max == rgb.b) {
            const float diff = (rgb.r - rgb.g) / cf;
            hue = 4 + diff / c;
        }
        hue *= 60.f;
        if (hue < 0) {
            hue *= 360.f;
        }
    }
    if (max == 0) {
        s = 0;
    } else {
        s = (max - min) / static_cast<float>(max);
    }
    v = max / 255.f;
}

inline hsv_t::hsv_t(const hsl_t hsl) : hue(hsl.hue) {
    // see https://en.wikipedia.org/wiki/HSL_and_HSV#HSL_to_HSV
    v = hsl.l + hsl.s * std::min(hsl.l, 1.f - hsl.l);
    s = v <= std::numeric_limits<float>::epsilon() * 2 ? 0 : 2 - 2 * hsl.l / v;
}
inline hsl_t::hsl_t(const hsv_t hsv) : hue(hsv.hue) {
    // see https://en.wikipedia.org/wiki/HSL_and_HSV#HSV_to_HSL
    l = hsv.v - hsv.v * hsv.s / 2.f;
    if (l <= std::numeric_limits<float>::epsilon() * 2 || (1.f - l) <= std::numeric_limits<float>::epsilon() * 2) {
        l = 0;
    } else {
        s = (hsv.v - l) / std::min(l, 1.f - l);
    }
}

inline hsv_t rgb_t::toHSV() const {
    return *this;
}
inline hsv_t hsl_t::toHSV() const {
    return *this;
}

inline rgb_t::rgb_t(const hsv_t hsv) {
    // see https://en.wikipedia.org/wiki/HSL_and_HSV#HSV_to_RGB_alternative
    const auto kf = [&](const auto n) { return std::fmod(n + hsv.h / 60, 6.f); };
    const auto f = [&](const auto n) {
        const auto k = kf(n);
        return hsv.v - hsv.v * hsv.s * std::max(std::min(1.f, std::min(k, 4 - k)), 0.f);
    };
    r = static_cast<brightness_t>(f(5) * 255.f);
    g = static_cast<brightness_t>(f(3) * 255.f);
    b = static_cast<brightness_t>(f(1) * 255.f);
}

/**
 * @brief typeToEnum return for the brightness_t and rgb_t the right enum value
 * @return the enum value represents the real type
 */
template <typename VT>
ValueType typeToEnum() {
    static_assert(std::is_same<VT, brightness_t>::value || std::is_same<VT, rgb_t>::value, "Not a value type");
    if constexpr (std::is_same<VT, brightness_t>::value) {
        return Brightness;
    }
    return RGB;
}

/**
 * @brief getSizeOfEnumType return for Brightness and RGB the size of the type brightness_t and rwg_t
 * @param t the type
 * @return the size of the type
 */
inline size_t getSizeOfEnumType(ValueType t) {
    switch (t) {
    case Brightness: return sizeof(brightness_t);
    case RGB: return sizeof(rgb_t);
    }
    assert(false && "Nicht im Enum");
    return 0;
}

class PropertyBase : public Serilerizeable {
protected:
    std::vector<Property *> properties;

public:
    const std::vector<Property *> &getProperties() const { return properties; }
    void load(const LoadObject &l) override {
        for (auto &p : properties) {
            p->load(l);
        }
    }
    void save(SaveObject &s) const override {
        for (auto &p : properties) {
            p->save(s);
        }
    }
};

class OutputDataProducer {
    const ValueType outputDataType;

public:
    OutputDataProducer(const ValueType outputDataType) : outputDataType(outputDataType) {}
    virtual ~OutputDataProducer() = default;
    inline ValueType getOutputType() const { return outputDataType; }
    virtual unsigned int getOutputLength() const = 0;
    /**
     * @brief getOutputData gibt einen Pointer auf die Ausgabedaten zurück, diese müssen sizeOf(Type) * outputLength groß sein
     * @return
     */
    virtual void *getOutputData() = 0;
};

class InputDataConsumer {
    const ValueType inputDataType;

public:
    InputDataConsumer(const ValueType inputDataType) : inputDataType(inputDataType) {}
    virtual ~InputDataConsumer() = default;
    /**
     * @brief getInputType gibt den Typ der Eingabedaten an.
     * @return brighness_t or rgb_t
     */
    inline ValueType getInputType() const { return inputDataType; }
    virtual unsigned int getInputLength() const = 0;
    /**
     * @brief setInputData setzt die eingabedaten, angezeigt werden sie mit show
     * @param data Die daten die angezeigt werden soll
     * @param index Der start index ab dem die daten in den input array geschrieben werden
     * @param length die anzahl der daten in void * data
     */
    virtual void setInputData(void *data, unsigned int index, unsigned int length) = 0;
};

/**
 * @brief The BoundedArray class maps index acces lower 0 to 0 and index access greater length-1 to length-1
 */
template <typename Type>
class BoundedArray {
    Type *data;
    size_t length;

public:
    BoundedArray(Type *data, size_t length) : data(data), length(length) {}
    Type &operator[](std::size_t idx) { return idx < length ? data[idx] : data[length - 1]; }
    const Type &operator[](std::size_t idx) const { return idx < length ? data[idx] : data[length - 1]; }
    Type &operator[](int i) {
        if (i >= 0) {
            if (i < length) {
                return data[i];
            } else {
                return data[length - 1];
            }
        } else {
            return data[0];
        }
    }
    const Type &operator[](int i) const {
        if (i >= 0) {
            if (i < length) {
                return data[i];
            } else {
                return data[length - 1];
            }
        } else {
            return data[0];
        }
    }
};
/**
 * @brief The WrappedArray class maps index acces lower 0 to length - i and index access greater length-1 to i - length
 */
template <typename Type>
class WrappedArray {
    Type *data;
    size_t length;

public:
    WrappedArray(Type *data, size_t length) : data(data), length(length) {}
    // Type& operator[](std::size_t idx)       { return idx<length?data[idx]:data[length-1]; }
    // const Type& operator[](std::size_t idx) const { return idx<length?data[idx]:data[length-1]; }
    Type &operator[](int i) {
        if (i >= 0) {
            if (i < length) {
                return data[i];
            } else {
                return data[i % length];
            }
        } else {
            return data[((i % static_cast<int>(length)) + length) % length];
        }
    }
    const Type &operator[](int i) const {
        if (i >= 0) {
            if (i < length) {
                return data[i];
            } else {
                return data[i % length];
            }
        } else {
            return data[((i % static_cast<int>(length)) + length) % length];
        }
    }
};

using namespace std::string_literals;

class IndexOutOfBoundsException : public std::exception {
public:
    const int min, max, index;
    std::string message;
    IndexOutOfBoundsException(int min, int max, int index)
        : min(min)
        , max(max)
        , index(index)
        , message("IndexOutOfBoundsException min: "s + std::to_string(min) + " max: " + std::to_string(max) + " your index: " + std::to_string(index)) {}
    virtual const char *what() const noexcept override { return message.c_str(); }
};

/**
 * @brief The CheckedArray class does index bounds checking
 */
template <typename Type>
class CheckedArray {
    size_t length;
    Type *data;

public:
    CheckedArray(Type *data, size_t length) : data(data), length(length) {}
    Type &operator[](std::size_t idx) { return idx < length ? data[idx] : throw IndexOutOfBoundsException(0, length - 1, idx); }
    const Type &operator[](std::size_t idx) const { return idx < length ? data[idx] : throw IndexOutOfBoundsException(0, length - 1, idx); }
    Type &operator[](int i) {
        if (i >= 0 && i < length) {
            return data[i];
        } else {
            throw IndexOutOfBoundsException(0, length - 1, i);
        }
    }
    const Type &operator[](int i) const {
        if (i >= 0 && i < length) {
            return data[i];
        } else {
            throw IndexOutOfBoundsException(0, length - 1, i);
        }
    }
};

class Named {
public:
    virtual const char *getName() const = 0;
    virtual ~Named() = default;
};

/**
 * Must be here, in the Property.hpp we have no rgb_t type
 * @brief The RGBProperty class is a Property wrapper araoud the rgb_t type
 */
class RGBProperty : public Property {
    rgb_t value;

public:
    RGBProperty() : Property(Property::RGB), value(0, 0, 0) {}

    void save(SaveObject &o) const override {
        auto red = name + "_red";
        auto green = name + "_green";
        auto blue = name + "_blue";
        o.saveInt(red.c_str(), value.red);
        o.saveInt(green.c_str(), value.green);
        o.saveInt(blue.c_str(), value.blue);
    }
    void load(const LoadObject &l) override {
        auto red = name + "_red";
        auto green = name + "_green";
        auto blue = name + "_blue";
        value.red = l.loadInt(red.c_str(), value.red);
        value.green = l.loadInt(green.c_str(), value.green);
        value.blue = l.loadInt(blue.c_str(), value.blue);
    }
    const rgb_t &operator*() const { return value; }
    rgb_t &operator*() { return value; }
    void setRGB(const rgb_t &v) { value = v; }
    rgb_t getRGB() const { return value; }
};

} // namespace Modules

#endif // TYPES_H
