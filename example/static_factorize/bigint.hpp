#pragma once

#include <boost/multiprecision/cpp_int.hpp>
//#include <boost/multiprecision/gmp.hpp>

#include "xcrypt/core/integer_traits.hpp"

using boost::multiprecision::cpp_int;
//using boost::multiprecision::gmp_int;

template <>
struct xcrypt::double_integer<cpp_int>
{
	using type = cpp_int;
};
//
//template <>
//struct xcrypt::double_integer<gmp_int>
//{
//	using type = gmp_int;
//};