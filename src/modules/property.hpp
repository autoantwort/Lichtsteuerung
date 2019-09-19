#ifndef VARIABLE_H
#define VARIABLE_H

#include "storage.hpp"
#include <stdexcept>
#include <string>
#include <utility>

namespace Modules {

template <typename ValueType> class NumericProperty;

class StringProperty;
class BoolProperty;
struct rgb_t;
class RGBProperty;

/**
 * @brief The Property class holds a property. A Property have a name and a description.
 */
class Property : public Serilerizeable {
protected:
    std::string name;
    std::string description;

public:
    const enum Type { Int = 0, Long = 1, Float = 2, Double = 3, Bool = 4, String = 5, RGB = 6 } type;

protected:
    Property(Type t) : type(t) {}

public:
    void setName(const std::string &_name) { name = _name; }
    [[nodiscard]] std::string getName() const { return name; }

    void setDescription(const std::string &_description) { description = _description; }
    [[nodiscard]] std::string getDescription() const { return description; }

    template <typename T> NumericProperty<T> *asNumeric();

    StringProperty *asString() {
        if (type == String) {
            return reinterpret_cast<StringProperty *>(this);
        }
        return nullptr;
    }

    BoolProperty *asBool() {
        if (type == Bool) {
            return reinterpret_cast<BoolProperty *>(this);
        }
        return nullptr;
    }

    RGBProperty *asRGB() {
        if (type == RGB) {
            return reinterpret_cast<RGBProperty *>(this);
        }
        return nullptr;
    }
};

namespace defail {

    template <typename...> constexpr std::false_type always_false{};

    template <typename t> Property::Type toEnum() {

        if constexpr (std::is_same_v<t, int>) {
            return Property::Int;
        } else if constexpr (std::is_same_v<t, int64_t>) {
            return Property::Long;
        } else if constexpr (std::is_same_v<t, float>) {
            return Property::Float;
        } else if constexpr (std::is_same_v<t, double>) {
            return Property::Double;
        } else if constexpr (std::is_same_v<t, std::string>) {
            return Property::String;
        } else if constexpr (std::is_same_v<t, rgb_t>) {
            return Property::RGB;
        } else if constexpr (std::is_same_v<t, bool>) {
            return Property::Bool;
        } else {
            static_assert(always_false<t>, "Wrong Type. Con be only int float long double bool std::string");
        }
    }
} // namespace defail

template <typename T> NumericProperty<T> *Property::asNumeric() {
    if (type == defail::toEnum<T>()) {
        return reinterpret_cast<NumericProperty<T> *>(this);
    }
    return nullptr;
}

template <typename Type_t> class NumericProperty : public Property {
    static_assert(std::is_same<Type_t, float>::value || std::is_same<Type_t, int>::value || std::is_same<Type_t, int64_t>::value ||
                      std::is_same<Type_t, double>::value,
                  "Wrong Type. Con only be signed numeric");
    Type_t min;
    Type_t max;
    Type_t value;

public:
    NumericProperty(Type_t min, Type_t max, Type_t value) : Property(defail::toEnum<Type_t>()), min(min), max(max), value(value) {}

    void load(const LoadObject &o) override {
        if constexpr (std::is_same_v<Type_t, int>) {
            value = o.loadInt(name.c_str(), value);
        } else if constexpr (std::is_same_v<Type_t, int64_t>) {
            value = o.loadLong(name.c_str(), value);
        } else if constexpr (std::is_same_v<Type_t, float>) {
            value = o.loadFloat(name.c_str(), value);
        } else if constexpr (std::is_same_v<Type_t, double>) {
            value = o.loadDouble(name.c_str(), value);
        } else {
            value = o.loadBool(name.c_str(), value);
        }
    }

    void save(SaveObject &s) const override {
        if constexpr (std::is_same_v<Type_t, int>) {
            s.saveInt(name.c_str(), static_cast<int>(value));
        } else if constexpr (std::is_same_v<Type_t, int64_t>) {
            s.saveLong(name.c_str(), static_cast<int64_t>(value));
        } else if constexpr (std::is_same_v<Type_t, float>) {
            s.saveFloat(name.c_str(), static_cast<float>(value));
        } else if constexpr (std::is_same_v<Type_t, double>) {
            s.saveDouble(name.c_str(), static_cast<double>(value));
        } else {
            s.saveBool(name.c_str(), static_cast<bool>(value));
        }
    }

    void setValue(const Type_t _value) { value = std::min(getMax(), std::max(_value, getMin())); }
    Type_t getValue() const { return value; }

    void setMin(const Type_t _min) { min = _min; }
    Type_t getMin() const { return min; }

    void setMax(const Type_t _max) { max = _max; }
    Type_t getMax() const { return max; }
};

class StringProperty : public Property {
protected:
    std::string value;

public:
    explicit StringProperty(std::string value) : Property(Property::String), value(std::move(value)) {}
    void save(SaveObject &o) const override { o.saveString(name.c_str(), value.c_str()); }
    void load(const LoadObject &l) override { value = l.loadString(name.c_str(), value); }
    /**
     * @brief setValuetry to set the value, if that fails, false is returned
     * @param value the new value
     * @return true  for succes
     */
    virtual bool setValue(const std::string &value) {
        this->value = value;
        return true;
    }
    [[nodiscard]] std::string getString() const { return value; }
    virtual ~StringProperty() = default;
};

class BoolProperty : public Property {
protected:
    bool value;

public:
    explicit BoolProperty(bool value) : Property(Property::Bool), value(value) {}
    void save(SaveObject &o) const override { o.saveBool(name.c_str(), value); }
    void load(const LoadObject &l) override { value = l.loadBool(name.c_str(), value); }

    void setValue(bool value) { this->value = value; }

    [[nodiscard]] bool getValue() const { return value; }
};

} // namespace Modules

#endif // VARIABLE_H
