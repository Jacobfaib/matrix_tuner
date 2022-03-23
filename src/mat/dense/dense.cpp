#include <matrix_tuner/matrix/dense.hpp>

namespace mt
{

dense_matrix::dense_matrix(const matrix &other) noexcept : base_type{other.nrows(),other.ncols()}
{
  data_.reserve(nrows_*ncols_);
  base_type::deep_copy(other);
}

mt_error_t dense_matrix::mult(const matrix *A, matrix *B) const noexcept MT_TRY(
{
  return MT_SUCCESS;
});

dense_matrix::const_reference_type dense_matrix::operator()(dense_matrix::index_type r, dense_matrix::index_type c) const noexcept
{
  return data_[(r*ncols_)+c];
}

dense_matrix::reference_type dense_matrix::operator()(dense_matrix::index_type r, dense_matrix::index_type c) noexcept
{
  return data_[(r*ncols_)+c];
}

mt_error_t dense_matrix::view(std::ostream& ostrm) const noexcept MT_TRY(
{
  for (auto i = 0; i < nrows_; ++i) {
    for (auto j = 0; j < ncols_-1; ++j) ostrm<<(*this)(i,j)<<',';
    ostrm<<(*this)(i,ncols_-1)<<'\n';
  }
  return MT_SUCCESS;
});

dense_matrix::index_type dense_matrix::nnz() const noexcept { return data_.size(); }

} // namespace mt
