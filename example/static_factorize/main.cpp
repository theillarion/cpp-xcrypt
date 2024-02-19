#include <iostream>
#include <chrono>

#include "xcrypt/factorize/qs.hpp"

#include "bigint.hpp"

int main()
{
	cpp_int n{ "2015638500984252513380255580199" };
	unsigned int B = 30000;
	std::size_t L = 5000000;

	xcrypt::QSFactorization<cpp_int> qs{B, L};

	auto start_time = std::chrono::steady_clock::now();
	auto success = qs.Perform(n);
	auto end_time = std::chrono::steady_clock::now();

	std::cout << std::endl << "B: " << qs.ParamB() << std::endl << "L: " << qs.ParamL() << std::endl;

	std::cout << "Count factor base: " << qs.FactorBase().size() << std::endl
			  << "Count B-smooth number: " << qs.SmoothNumbers().size() << std::endl;

	if (success)
	{
		auto time_perform = std::chrono::duration<long double>(end_time - start_time);

		std::cout << "\033[92mSuccess!\033[0m " << n << " = " << qs.Result1() << " * " << qs.Result2() << std::endl
				  << "Time: " << time_perform.count() << " seconds" << std::endl;
	}
	else
	{
		std::cout << "\033[91mFailure\033[0m: " << qs.State().Str() << std::endl;
	}
	return 0;
}