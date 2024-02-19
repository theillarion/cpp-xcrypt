#pragma once

#include <type_traits>

namespace xcrypt
{
	template<typename Integer1, typename Integer2>
	inline constexpr typename std::enable_if<std::is_unsigned<Integer1>::value && std::is_unsigned<Integer2>::value, Integer2>::type
	Mod(const Integer1& number, const Integer2& module)
	{
		return number % module;
	}
	template<typename Integer1, typename Integer2>
	inline constexpr typename std::enable_if<!(std::is_unsigned<Integer1>::value && std::is_unsigned<Integer2>::value), Integer2>::type
	Mod(const Integer1& number, const Integer2& module)
	{
		Integer1 div = number % static_cast<Integer1>(module);
		if (div < 0)
			return div + module;
		return div;
	}

	template <typename Integer>
	inline constexpr typename std::enable_if<std::is_unsigned<Integer>::value, Integer>::type
	Abs(const Integer& value)
	{
		return value;
	}

	template <typename Integer>
	inline constexpr typename std::enable_if<!std::is_unsigned<Integer>::value, Integer>::type
	Abs(const Integer& value)
	{
		return (value < 0) ? -value : value;
	}
}