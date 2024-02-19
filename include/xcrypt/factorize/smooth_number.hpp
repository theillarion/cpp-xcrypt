#pragma once

#include <utility>
#include <ostream>

namespace xcrypt
{
	template <typename Int>
	struct SmoothNumber
	{
		SmoothNumber() = default;
		~SmoothNumber() noexcept = default;
		SmoothNumber(const SmoothNumber&) = default;
		SmoothNumber(SmoothNumber&&) noexcept = default;

		SmoothNumber(Int x, Int smooth) : x{ std::move( x ) }, smooth{ std::move( smooth ) } { }

		SmoothNumber& operator=(const SmoothNumber&) = default;
		SmoothNumber& operator=(SmoothNumber&&) noexcept = default;

		//		friend void swap(SmoothNumber& lhs, SmoothNumber& rhs) noexcept
		//		{
		//			using std::swap;
		//
		//			swap(lhs.x, rhs.x);
		//			swap(lhs.smooth, rhs.smooth);
		//		}

		Int x;
		Int smooth;
	};

	template <typename Int>
	std::ostream& operator<<(std::ostream& out, const SmoothNumber<Int>& value)
	{
		return out << "{" << value.x << "; " << value.smooth << "}";
	}
}