#ifndef SYSTEM_ERROR_HANDLER_H
#define SYSTEM_ERROR_HANDLER_H

#include <boost/stacktrace.hpp>
#include <stdexcept>
#include <string>
#include <string_view>

namespace error {

class crash_error : public std::exception {
    std::string error;
    boost::stacktrace::stacktrace stacktrace;

public:
    explicit crash_error(std::string_view errorName);
    [[nodiscard]] const char *what() const noexcept override { return error.c_str(); }
    [[nodiscard]] const boost::stacktrace::stacktrace &getStacktrace() const noexcept { return stacktrace; }
};

void initErrorHandler();

} // namespace error

#endif // SYSTEM_ERROR_HANDLER_H
