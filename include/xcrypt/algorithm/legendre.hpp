#pragma once

#include <cassert>

#include "xcrypt/algorithm/fast_pow_mod.hpp"

namespace xcrypt
{
	template <typename Int, typename UInt>
	Int Legendre(Int a, UInt p)
	{
		Int result = FastPowMod<Int, UInt, UInt>(a, (p - 1) / 2, p);
		if (result == 1)
			return 1;
		else if (result == p - 1)
			return -1;
		else if (result == 0)
			return 0;

		assert("incorrect a or p");
		return 0;
	}
}