#ifndef SPAN_H
#define SPAN_H

// TODO use std::span in c++20
#ifndef USE_SPAN_LITE
#include <boost/beast/core/span.hpp>

template <typename T>
using span = boost::beast::span<T>;
#else
#include <nonstd/span.hpp>

template <typename T>
using span = nonstd::span<T>;
#endif

#endif // SPAN_H
