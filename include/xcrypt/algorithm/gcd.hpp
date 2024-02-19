#pragma once

#include <type_traits>
#include <tuple>
#include <stack>
#include <stdexcept>

#include "xcrypt/algorithm/utilities.hpp"

namespace
{
	template <typename Integer>
	Integer GcdImpl(Integer a, Integer b)
	{
		using std::swap;

		Integer prev_a{ 0 };
		if (a < b)
			swap(a, b);

		while (b != 0)
		{
			prev_a = a;
			a = b;
			b = prev_a % b;
		}
		return a;
	}

	template<typename Integer>
	std::tuple<Integer, Integer, Integer> GcdExtendedImpl(const Integer& a, const Integer& b)
	{
		using std::swap;

		std::stack<Integer>	divs{ };
		Integer             local_a{ a };
		Integer             local_b{ b };
		Integer				s_old{ 0 };
		Integer				mod{ 0 };
		Integer				s{ 0 };
		Integer				d{ 1 };

		if (local_a == 0)
			return std::make_tuple(local_b, s, d);
		divs.push(local_a / local_b);
		mod = local_a % local_b;
		while (mod)
		{
			local_a = local_b;
			local_b = mod;
			divs.push(local_a / local_b);
			mod = local_a % local_b;
		}
		divs.pop();
		while (!divs.empty())
		{
			s_old = s;
			s = d;
			d = s_old - d * divs.top();
			divs.pop();
		}
		return (std::make_tuple(local_b, s, d));
	}
}

namespace xcrypt
{
	template <typename Integer>
	inline constexpr typename std::enable_if<std::is_unsigned<Integer>::value, Integer>::type
	Gcd(Integer a, Integer b)
	{
		return GcdImpl(a, b);
	}

	template <typename Integer>
	inline constexpr typename std::enable_if<!std::is_unsigned<Integer>::value, Integer>::type
	Gcd(Integer a, Integer b)
	{
		return GcdImpl(Abs(a), Abs(b));
	}

	template<typename Integer>
	inline typename std::enable_if<std::is_unsigned<Integer>::value, std::tuple<Integer, Integer, Integer>>::type
	GcdExtended(const Integer& a, const Integer& b)
	{
		return GcdExtendedImpl(a, b);
	}

	template<typename Integer>
	inline typename std::enable_if<!std::is_unsigned<Integer>::value, std::tuple<Integer, Integer, Integer>>::type
	GcdExtended(const Integer& a, const Integer& b)
	{
		if (a < 1 || b < 1)
			throw std::invalid_argument("arguments must be strictly positive");
		return GcdExtendedImpl(a, b);
	}
}