#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <cstdint>
#include <string>

namespace Modules {

/**
 * @brief The SaveObject interface defines Methods to save Primitive data
 */
class SaveObject
{
public:
    virtual void saveInt(const char *name, int) = 0;
    virtual void saveFloat(const char *name, float) = 0;
    virtual void saveDouble(const char *name, double) = 0;
    virtual void saveBool(const char *name, bool) = 0;
    virtual void saveLong(const char *name, int64_t) = 0;
    virtual void saveString(const char *name, const char *c) = 0;
};

/**
 * @brief The LoadObject interface defines methods to load Primitive data
 */
class LoadObject
{
public:
    virtual int loadInt(const char *name, int defaultValue) const = 0;
    virtual float loadFloat(const char *name, float defaultValue) const = 0;
    virtual double loadDouble(const char *name, double defaultValue) const = 0;
    virtual bool loadBool(const char *name, bool defaultValue) const = 0;
    virtual int64_t loadLong(const char *name, int64_t defaultValue) const = 0;
    /**
     * @brief loadString loads a string
     * @param name the name of the property
     * @param defaultValue the value that is returned if the property with name name does not exists
     * @return the string or the default value if the property does not exist
     */
    virtual std::string loadString(const char *name, std::string defaultValue) const = 0;
};

/**
 * @brief A Superclass for objects that can be load and stored
 */
class Serilerizeable{
public:
    virtual void save(SaveObject &s) const = 0;
    virtual void load(const LoadObject &l) = 0;
};

} // namespace Modules

#endif // STORAGE_HPP
