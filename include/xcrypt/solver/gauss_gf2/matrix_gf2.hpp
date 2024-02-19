#pragma once

#include <ostream>

#include <boost/multiprecision/integer.hpp>

#include "xcrypt/core/types.hpp"

namespace xcrypt
{
	namespace internal = boost::multiprecision;
	template <typename Int>
	class MatrixGF2;
}

namespace
{
	using namespace xcrypt;

	template <typename Int>
	class ProxyRowGF2
	{
	public:
		friend class MatrixGF2<Int>;

	private:
		explicit ProxyRowGF2(const Int& ref_data, index_type cols) : ref_data_{ ref_data }, cols_{ cols } { }

	public:
		bool operator[](index_type idx_col) const { return internal::bit_test(ref_data_, idx_col); }
		const Int& operator*() const { return ref_data_; }
		const Int* operator->() const { return &ref_data_; }
		const Int& value() const { return ref_data_; }
		Int pure_value() const
		{
			Int temp = ref_data_;;
			return xcrypt::internal::bit_unset(temp, cols_);
		}

	private:
		const Int& ref_data_;
		index_type cols_;
	};
}

namespace xcrypt
{
	template <typename Int>
	class MatrixGF2
	{
	private:
		using ProxyRowSelf = ProxyRowGF2<Int>;
		using Self = MatrixGF2<Int>;

	public:
		MatrixGF2(index_type rows, index_type cols)
			:	rows_{ rows }, cols_{ cols }, data_(rows_)
		{
			for (index_type i = 0; i < rows; ++i)
				BitSet(i, cols_);
		}

		static Self One(index_type rows, index_type cols)
		{
			MatrixGF2 result(rows, cols);

			for (index_type i = 0; i < rows; ++i)
				result[i] = (1 << (cols + 1)) - 1;

			return result;
		}

		static Self Identity(index_type rows_cols)
		{
			MatrixGF2 result(rows_cols, rows_cols);

			for (index_type i = 0; i < rows_cols; ++i)
				result.BitSet(i, i);

			return result;
		}

		void Init(index_type rows, index_type cols)
		{
			this->rows_ = rows;
			this->cols_ = cols;
			this->data_.resize(rows, Int{ 0 });
		}

		const auto& GetStorage() const { return data_; }

		[[nodiscard]] index_type Cols() const { return cols_; }
		[[nodiscard]] index_type Rows() const { return rows_; }

		void RowsSwap(index_type idx_row1, index_type idx_row2)
		{
			internal::swap(data_[idx_row1], data_[idx_row2]);
		}

		void RowsXor(index_type idx_dst_row, index_type idx_src_row1, index_type idx_src_row2)
		{
			data_[idx_dst_row] = data_[idx_src_row1] ^ data_[idx_src_row2];
		}

		[[nodiscard]] bool RowsCmp(index_type idx_row, std::int64_t op) const
		{
			Int temp = data_[idx_row];
			return internal::bit_unset(temp, cols_) == Int{ op };
		}

		[[nodiscard]] bool BitTest(index_type idx_row, index_type idx_col) const { return internal::bit_test(data_[idx_row], idx_col); }
		void BitSet(index_type idx_row, index_type idx_col) { internal::bit_set(data_[idx_row], idx_col); }
		void BitUnSet(index_type idx_row, index_type idx_col) { internal::bit_unset(data_[idx_row], idx_col); }
		void BitFlip(index_type idx_row, index_type idx_col) { internal::bit_flip(data_[idx_row], idx_col); }

		Int& operator[](index_type row) { return data_[row]; }
		ProxyRowSelf operator[](index_type row) const { return ProxyRowSelf(data_[row], cols_); }

	private:
		index_type			rows_;
		index_type			cols_;
		std::vector<Int>	data_;
	};

	template <typename Int>
	std::ostream& operator<<(std::ostream& out, const MatrixGF2<Int>& other)
	{
		for (index_type i = 0; i < other.Rows(); ++i)
		{
			for (index_type j = 0; j < other.Cols(); ++j)
			{
				out << other[i][j];
				if (j != other.Cols() - 1)
					out << ' ';
			}
			out << std::endl;
		}
		return out;
	}
}