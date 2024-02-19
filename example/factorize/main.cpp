#include <iostream>
#include <chrono>
#include <charconv>
#include <cstring>

#include "bigint.hpp"
#include "xcrypt/factorize/qs.hpp"

int main(int argc, char **argv)
{
	if (argc != 4)
	{
		std::cerr << "\033[91mError\033[0m: wrong number of arguments\n"
			<< "./" << argv[0] << " B L RSA_number" << std::endl;
		return 1;
	}

	unsigned int	b = 0;
	unsigned int	l = 0;
	cpp_int			n = 1;

	if (std::from_chars(argv[1], argv[1] + std::strlen(argv[1]), b).ec != std::errc{})
	{
		std::cerr << "\033[91mError\033[0m: incorrect B" << std::endl;
		return 2;
	}

	if (std::from_chars(argv[2], argv[2] + std::strlen(argv[2]), l).ec != std::errc{})
	{
		std::cerr << "\033[91mError\033[0m: incorrect L" << std::endl;
		return 2;
	}

	try
	{
		n = cpp_int{ argv[3] };
	}
	catch (...)
	{
		std::cerr << "\033[91mError\033[0m: incorrect RSA_number" << std::endl;
		return 2;
	}

	xcrypt::QSFactorization<cpp_int> qs{b, l};

	auto start_time = std::chrono::steady_clock::now();
	auto success = qs.Perform(n);
	auto end_time = std::chrono::steady_clock::now();

	std::cout << "N = " << n << std::endl << "B: " << qs.ParamB() << std::endl << "L: " << qs.ParamL() << std::endl;

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