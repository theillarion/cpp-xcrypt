#pragma once

#include <span>

#include "xcrypt/solver/gauss_gf2/matrix_gf2.hpp"

// ====== DEFINITION SolverGaussGF2 ====== //
namespace xcrypt
{
	template <typename Int>
	class SolverGaussGF2
	{
	private:
		using matrix_type = MatrixGF2<Int>;
		using relations_cont_type = std::vector<Int>;

		using Self = SolverGaussGF2<Int>;
	public:

		explicit SolverGaussGF2(matrix_type matrix) : matrix_{ std::move(matrix) }, linear_relations_{ } { }

		template <typename IntFactor, typename SmoothNumber>
		static Self CreateSolver(std::span<std::add_const_t<IntFactor>> factor_base,
								 std::span<std::add_const_t<SmoothNumber>> smooth_numbers)
		{
			matrix_type result_matrix_(smooth_numbers.size(), factor_base.size());

			for (std::size_t i = 0; i < smooth_numbers.size(); ++i)
			{
				auto smooth_number = smooth_numbers[i].smooth;
				for (std::size_t j = 0; j < factor_base.size(); ++j)
				{
					std::int32_t curr_factor{ 0 };
					while (smooth_number % factor_base[j] == 0)
					{
						smooth_number /= factor_base[j];
						++curr_factor;
					}
					if (curr_factor % 2 == 1)
						result_matrix_.BitSet(i, j);
				}
			}

			return Self(std::move(result_matrix_));
		}

		bool Solve()
		{
			auto identity_matrix = matrix_type::Identity(matrix_.Rows());

			index_type curr_non_reduced_row = 0;
			index_type idx_curr_pivot;

			for (index_type j = 0; j < matrix_.Cols(); ++j)
			{
				auto idx_next_pivot = static_cast<index_type>(-1);

				for (index_type i = curr_non_reduced_row; i < matrix_.Rows(); ++i)
				{
					if (matrix_.BitTest(i, j))
					{
						idx_next_pivot = i;
						break;
					}
				}

				//	Now candidate row found, continue to next column
				if (idx_next_pivot == static_cast<index_type>(-1))
					continue;

				//	The pivot is different from the current non-reduced row, switch them!
				if(idx_next_pivot != curr_non_reduced_row)
				{
					matrix_.RowsSwap(idx_next_pivot, curr_non_reduced_row);
					identity_matrix.RowsSwap(idx_next_pivot, curr_non_reduced_row);
				}

				idx_curr_pivot = curr_non_reduced_row;
				++curr_non_reduced_row;

				for (index_type i = curr_non_reduced_row; i < matrix_.Rows(); ++i)
				{
					if (matrix_.BitTest(i, j))
					{
						matrix_.RowsXor(i, i, idx_curr_pivot);
						identity_matrix.RowsXor(i, i, idx_curr_pivot);
					}
				}
			}

			return FillLinearRelations(identity_matrix);
		}

		bool FillLinearRelations(const matrix_type& identity_matrix)
		{
			if (identity_matrix.Rows() < 1)
				return false;

			//assert(this->matrix_.Rows() == identity_matrix.Rows());

			std::ptrdiff_t idx_last_row = identity_matrix.Rows() - 1;
			index_type count_lin_relations = 0;

			while (idx_last_row > -1 && this->matrix_.RowsCmp(idx_last_row, 0))
			{
				--idx_last_row;
				++count_lin_relations;
			}

			if (count_lin_relations < 1)
				return false;

			idx_last_row = identity_matrix.Rows() - 1;

			for(int i = 0; i < count_lin_relations; ++i)
				this->linear_relations_.push_back(identity_matrix[idx_last_row - i].pure_value());

			return true;
		}

		const relations_cont_type& GetLinearRelations() const { return linear_relations_; }
		auto GetCountLinearRelations() const { return linear_relations_.size(); }

	private:
		matrix_type			matrix_;
		relations_cont_type	linear_relations_;
	};
}