#pragma once

#include <cstdint>
#include <vector>
#include <array>
#include <random>

#include <boost/random/uniform_int_distribution.hpp>

#include "xcrypt/core/types.hpp"
#include "xcrypt/algorithm/algorithm.hpp"
#include "xcrypt/factorize/smooth_number.hpp"
#include "xcrypt/solver/gauss_gf2.hpp"

namespace xcrypt
{
	template<typename Int>
	class QSFactorization
	{
	public:
		using int_type = Int;
		using factor_type = unsigned int;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		class Result;

	private:
		template <typename T>
		using cont_type = std::vector<T>;

		using fbase_cont_type = cont_type<factor_type>;
		using smooth_cont_type = cont_type<SmoothNumber<int_type>>;

	public:
		QSFactorization() = default;
		QSFactorization(factor_type b, size_type l)
			:	result_{ Result::Success }, param_b_{ b }, param_l_{ l },
				factor_base_{ }, smooth_numbers_{ }, result1_{ 0 }, result2_{ 0 }
		{ }

		bool Perform(int_type n)
		{
			if (param_b_ == 0 || param_l_ == 0)
			{
				result_.State() = Result::NotSetParams;
				return false;
			}

			if (auto x = sqrt(n); x * x == n)
			{
				result_.State() = Result::Success;
				result1_ = x; result2_ = x;
				return true;
			}

			factor_base_ = GetFactorBase(n, param_b_);
			Sieve(n);

			if (factor_base_.size() > smooth_numbers_.size())
			{
				result_.State() = Result::LowSmoothNumber;
				return false;
			}

			using type1 = typename fbase_cont_type::value_type;
			using type2 = typename smooth_cont_type::value_type;
			auto solver = SolverGaussGF2<int_type>::template CreateSolver<type1, type2>(factor_base_, smooth_numbers_);

			auto result_solve = solver.Solve();
			if (!result_solve)
			{
				result_.State() = Result::NotFoundNonTrivialSolutions;
				return false;
			}

			return SolutionSearch(n, solver);
		}

		inline void SetParamB(factor_type new_b) { param_b_ = new_b; }
		inline void SetParamL(size_type new_l) { param_l_ = new_l; }

		inline factor_type ParamB() const noexcept { return param_b_; }
		inline size_type ParamL() const noexcept { return param_l_; }
		[[nodiscard]] inline Result State() const noexcept { return result_; }
		inline const int_type& Result1() const noexcept { return result1_; }
		inline const int_type& Result2() const noexcept { return result2_; }
		inline const fbase_cont_type& FactorBase() const noexcept { return factor_base_; }
		inline const smooth_cont_type& SmoothNumbers() const noexcept { return smooth_numbers_; }

	private:
		template <typename Int1, typename Int2>
		std::vector<Int2> GetFactorBase(Int1 n, Int2 upper_bound)
		{
			using uniform_dist = boost::random::uniform_int_distribution<Int2>;
			std::mt19937 gen{ std::random_device{ }() };
			std::vector<Int2> result;
			result.push_back(2);

			for (Int2 i = 3; i < upper_bound; i += 2)
			{
				// TODO: reduce reliance on Boost
				if (xcrypt::TestMillerRabin<uniform_dist>(i, boost::multiprecision::msb(i), gen) == true)
				{
					if (xcrypt::Legendre(n, i) == 1)
						result.push_back(i);
				}
			}
			return result;
		}

		void Sieve(int_type n)
		{
			index_type max_smooth_number =
						   (factor_base_.size() < std::numeric_limits<index_type>::max() - 9)
						   ? factor_base_.size() + 10
						   : std::numeric_limits<index_type>::max();

			assert(factor_base_[0] > 0);

			smooth_numbers_.reserve(param_l_ + 1);

			Int x = sqrt(n) + 1;
			for (size_type i = 0; i <= param_l_; ++i, ++x)
				smooth_numbers_.emplace_back(x, x * x - n);

			index_type	count_smooth = 0;
			for (const auto& factor: factor_base_)
			{
				// Find start position first chain
				std::size_t idx_first_chain = 0;
				while (idx_first_chain < smooth_numbers_.size() && smooth_numbers_[idx_first_chain].smooth % factor != 0)
					++idx_first_chain;

				// Find start position second chain
				std::size_t idx_second_chain = idx_first_chain + 1;
				while (idx_second_chain < smooth_numbers_.size() && smooth_numbers_[idx_second_chain].smooth % factor != 0)
					++idx_second_chain;

				// Main loop first chain
				for (; idx_first_chain < smooth_numbers_.size(); idx_first_chain += factor)
				{
					while (smooth_numbers_[idx_first_chain].smooth % factor == 0)
						smooth_numbers_[idx_first_chain].smooth /= factor;

					if (smooth_numbers_[idx_first_chain].smooth == 1)
						++count_smooth;
					if (count_smooth == max_smooth_number)
						break;
				}

				if (count_smooth == max_smooth_number)
					break;

				if (factor < 3)
					continue;

				// Main loop second chain
				for (;idx_second_chain < smooth_numbers_.size(); idx_second_chain += factor)
				{
					while (smooth_numbers_[idx_second_chain].smooth % factor == 0)
						smooth_numbers_[idx_second_chain].smooth /= factor;

					if (smooth_numbers_[idx_second_chain].smooth == 1)
						++count_smooth;
					if (count_smooth == max_smooth_number)
						break;
				}

				if (count_smooth == max_smooth_number)
					break;
			}

			auto remove_no_smooth = [](const auto &elem) { return elem.smooth != 1; };
			auto new_end = std::remove_if(std::begin(smooth_numbers_), std::end(smooth_numbers_), remove_no_smooth);
			smooth_numbers_.erase(new_end, std::end(smooth_numbers_));
			smooth_numbers_.shrink_to_fit();

			for (auto& smooth_number: smooth_numbers_)
				smooth_number.smooth = smooth_number.x * smooth_number.x - n;
		}

		bool SolutionSearch(int_type n, const SolverGaussGF2<int_type>& solver)
		{
			namespace internal = boost::multiprecision;

			if (solver.GetCountLinearRelations() < 1)
			{
				result_.State() = Result::NotFoundNonTrivialSolutions;
				return false;
			}

			if (factor_base_.size() > smooth_numbers_.size())
			{
				result_.State() = Result::LowSmoothNumber;
				return false;
			}

			int_type x, y, non_trivial_divisor1, non_trivial_divisor2 = 1;

			auto relations = solver.GetLinearRelations();
			for (std::size_t i = 0; i < solver.GetCountLinearRelations(); ++i)
			{
				x = 1; y = 1;
				for (std::size_t j = 0; j < factor_base_.size(); ++j)
				{
					if (internal::bit_test(relations[i], j))
					{
						x *= smooth_numbers_[j].x;
						x %= n;

						y *= smooth_numbers_[j].smooth;
					}
				}
				y = internal::sqrt(y);
				y %= n;

				x = x - y;
				non_trivial_divisor1 = xcrypt::Gcd(x, n);
				if (non_trivial_divisor1 != 1 && non_trivial_divisor1 != n)
					break;
			}

			if (non_trivial_divisor1)
				non_trivial_divisor2 = n / non_trivial_divisor1;
			if (non_trivial_divisor1 == 1 || non_trivial_divisor1 == n || non_trivial_divisor1 * non_trivial_divisor2 != n)
			{
				result_.State() = Result::NotFoundNonTrivialDivisor;
				result1_ = n;
				result2_ = 1;
				return false;
			}

			std::tie(result2_, result1_) = std::minmax(non_trivial_divisor1, non_trivial_divisor2);
			result_.State() = Result::Success;
			return true;
		}

	private:
		Result				result_ { Result::Success };
		factor_type 		param_b_{ 0 };
		size_type			param_l_{ 0 };
		fbase_cont_type		factor_base_;
		smooth_cont_type	smooth_numbers_;
		int_type			result1_;
		int_type			result2_;
	};
}

namespace xcrypt
{
	template <typename Int>
	class QSFactorization<Int>::Result
	{
	public:
		using enum_type = std::int8_t;

		enum Type : enum_type
		{
			Success,
			NotSetParams,
			LowSmoothNumber,
			NotFoundNonTrivialSolutions,
			NotFoundNonTrivialDivisor
		};

	private:
		static constexpr enum_type size_enum = static_cast<enum_type>(NotFoundNonTrivialDivisor) + 1;
		static constexpr std::array<const char*, size_enum> strs
		{
		   "success",
		   "parameters B and L not set",
		   "small number of B-smooth integers",
		   "non-trivial solutions not found",
		   "non-trivial divisor not found"
		};

	public:
		explicit Result(Type state) : state_{ state } { }

		inline Type State() const noexcept { return state_; }
		inline Type& State() { return state_; }
		inline const char* Str() const noexcept { return strs[static_cast<enum_type>(state_)]; }
		static const char* Str(Type state) noexcept { return strs[static_cast<enum_type>(state)]; }
	private:
		Type state_;
	};
}
