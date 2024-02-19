#pragma once

#include <boost/multiprecision/cpp_int.hpp>
#include "xcrypt/core/integer_traits.hpp"

using boost::multiprecision::cpp_int;

template <> struct xcrypt::double_integer<cpp_int> { using type = cpp_int; };