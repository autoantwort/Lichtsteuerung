#include "system_error_handler.h"

#include <segvcatch.h>
#include <sstream>

error::crash_error::crash_error(std::string_view errorName) : error(errorName) {}

void throw_segv_error() {
    using namespace std::literals;
    throw error::crash_error("Segmention fault"sv);
}

void throw_div_zero_error() {
    using namespace std::literals;
    throw error::crash_error("Divide by zero"sv);
}

void error::initErrorHandler() {
    segvcatch::init_segv(throw_segv_error);
    segvcatch::init_fpe(throw_div_zero_error);
}
