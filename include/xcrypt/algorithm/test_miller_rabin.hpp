#pragma once

#include <cstdint>
#include <type_traits>
#include <tuple>

#include "xcrypt/core/integer_traits.hpp"
#include "xcrypt/algorithm/fast_pow_mod.hpp"

namespace
{
	using namespace xcrypt;

	template<typename Unsigned>
	constexpr std::tuple<Unsigned, Unsigned> NumberDecomposition(Unsigned number)
	{

		Unsigned d {number - 1};
		Unsigned s {0};

		while (d % 2 == 0)
		{
			d /= 2;
			++s;
		}
		return std::make_tuple(s, d);
	}

	template<typename Distribution, typename Integer, typename Generator>
	bool TestMillerRabinImpl(Integer number, unsigned int rounds, Generator gen)
	{

		Distribution dist(2, number - 2);
		Integer      a {0};
		Integer      x {0};
		bool         is_find {false};

		auto [s, d] = NumberDecomposition(number);
		for (unsigned int i      = 0; i < rounds; ++i)
		{
			is_find = false;
			a       = static_cast<Integer>(dist(gen));
			x       = FastPowMod(a, d, number);
			if (x == 1 || x == number - 1)
				continue;
			for (Integer j = 0; j < s - 1; ++j)
			{
				x = FastPowMod(x, Integer(2), number);
				if (x == 1)
					return (false);
				if (x == number - 1)
				{
					is_find = true;
					break;
				}
			}
			if (!is_find)
				return false;
		}
		return (true);
	}

	template<typename Integer>
	bool IsSmallPrime(const Integer &number)
	{
		static constexpr std::uint8_t primes[] =
		{
			2u, 3u, 5u, 7u, 11u, 13u, 17u, 19u, 23u, 29u, 31u, 37u, 41u, 43u, 47u /*...*/
		};
		static constexpr std::uint8_t count = sizeof(primes) / sizeof(*primes);

		if (number > primes[count - 1])
			return false;

		for (auto prime : primes)
			if (number == prime)
				return true;
		return false;
	}

	template<typename Integer>
	bool TestSmallFactors(const Integer &number)
	{
		static constexpr std::uint8_t primes[] = {2u, 3u, 5u, 7u, 11u, 13u};
		for (auto prime: primes)
			if (number % prime == 0)
				return false;
		return true;
	}
}

namespace xcrypt
{
	template <typename Distribution, typename Integer, typename Generator>
	inline constexpr bool
	TestMillerRabin(Integer number, unsigned int rounds, Generator gen)
	{
		if (number < 2)
			return false;
		if (IsSmallPrime(number))
			return true;
		return TestSmallFactors(number) && TestMillerRabinImpl<Distribution>(number, rounds, gen);
	}
}