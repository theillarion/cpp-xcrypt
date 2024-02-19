#pragma once

#include "xcrypt/core/integer_traits.hpp"

namespace
{
	template <typename DoubleInteger, typename Integer>
	inline DoubleInteger safe_multiply(const Integer& lhs, const Integer& rhs)
	{
		return static_cast<DoubleInteger>(lhs) * static_cast<DoubleInteger>(rhs);
	}

	template <typename T1, typename T2>
	inline T2 safe_mod(const T1& lhs, const T2& rhs)
	{
		return static_cast<T2>(lhs % rhs);
	}

	template <typename DoubleInteger, typename Integer1, typename Integer2, typename Integer3>
	constexpr Integer1
	FastPowModImpl(Integer1 base, Integer2 exp, const Integer3& module)
	{
		DoubleInteger   temp;
		Integer1   result = 1;

		if (base == 1 || exp == 0)
			return 1 % module;
		while (exp > 0)
		{
			if (exp & 1)
			{
				temp = safe_multiply<DoubleInteger>(base, result);
				result = safe_mod(temp, module);
			}
			exp >>= 1;
			temp = safe_multiply<DoubleInteger>(base, base);
			base = safe_mod(temp, module);
		}
		return result;
	}
} // namespace

namespace xcrypt
{
	template <typename DoubleInteger, typename Integer1, typename Integer2, typename Integer3>
	inline constexpr typename std::enable_if<(sizeof(DoubleInteger) >= sizeof(Integer1)), Integer1>::type
	FastPowMod(Integer1 base, Integer2 exp, const Integer3& module)
	{
		return FastPowModImpl<DoubleInteger>(base, exp, module);
	}

	template <typename Integer1, typename Integer2, typename Integer3>
	inline constexpr Integer1
	FastPowMod(Integer1 base, Integer2 exp, const Integer3& module)
	{
		return FastPowModImpl<double_integer_t<Integer1>>(base, exp, module);
	}
}