#include <matrix_tuner/matrix/dense.hpp>

namespace mt
{

mt_error_t dense_matrix::mult(const matrix *A, matrix *B) const noexcept
{
  return 0;
}

dense_matrix::const_reference_type dense_matrix::operator()(dense_matrix::index_type r, dense_matrix::index_type c) const noexcept
{
  return data_[(r*nrows_)+(c*ncols_)];
}

dense_matrix::reference_type dense_matrix::operator()(dense_matrix::index_type r, dense_matrix::index_type c) noexcept
{
  return data_[(r*nrows_)+(c*ncols_)];
}

mt_error_t dense_matrix::view(std::ostream& ostrm) const noexcept
{
  for (auto i = 0; i < nrows_; ++i) {
    for (auto j = 0; j < ncols_-1; ++j) ostrm<<(*this)(i,j)<<',';
    ostrm<<(*this)(i,ncols_-1)<<'\n';
  }
  return 0;
}
} // namespace mt
