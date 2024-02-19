#pragma once

#include <type_traits>

namespace xcrypt
{
	template <typename Integer/*, typename = std::enable_if<std::is_integral<Integer>::value>::type*/>
	struct double_integer
	{
		using type = void;
		//static_assert(false);
	};

	template<typename Integer>
	using double_integer_t = typename double_integer<Integer>::type;

	template <> struct double_integer<std::int8_t> { using type  = std::int16_t; };
	template <> struct double_integer<std::int16_t> { using type  = std::int32_t; };
	template <> struct double_integer<std::int32_t> { using type  = std::int64_t; };

	template <> struct double_integer<std::uint8_t> { using type  = std::uint16_t; };
	template <> struct double_integer<std::uint16_t> { using type  = std::uint32_t; };
	template <> struct double_integer<std::uint32_t> { using type  = std::uint64_t; };

	template <typename Integer/*, typename = std::enable_if<std::is_integral<Integer>::value>::type*/>
	struct make_unsigned
	{
		using type = typename std::make_unsigned<Integer>::type;
		//static_assert(false);
	};

	template<typename Integer>
	using make_unsigned_t = typename make_unsigned<Integer>::type;

	template <typename Integer/*, typename = std::enable_if<std::is_integral<Integer>::value>::type*/>
	struct make_signed
	{
		using type = typename std::make_signed<Integer>::type;
		//static_assert(false);
	};

	template<typename Integer>
	using make_signed_t = typename make_signed<Integer>::type;
}