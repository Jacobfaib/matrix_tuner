#include <matrix_tuner/matrix/csr.hpp>

namespace mt
{

csr_matrix::csr_matrix(const matrix &other) noexcept : base_type{other.nrows(),other.ncols()}
{
  base_type::deep_copy(other);
}

mt_error_t csr_matrix::mult(const matrix *A, matrix *B) const noexcept MT_TRY(
{
  return MT_SUCCESS;
});

mt_error_t csr_matrix::mult(const csr_matrix *A, csr_matrix *B) const noexcept MT_TRY(
{
  const auto nr  = nrows();
  const auto nc  = ncols();
  const auto anr = A->nrows();
  const auto anc = A->ncols();

  if (mtunlikely(nc != anr)) MTSETERR(MT_ERROR_INCOMP_SIZE,"Dimensions mismatch (%zu,%zu) not compatible with (%zu,%zu)",nr,nc,anr,anc);
  if (!B) B = new csr_matrix{nr,anc};

  for (index_type i = 1; i <= nr; ++i) {
    for (index_type j = 1; j <= anc; ++j) {
      auto a = value_type{};
      for (index_type k = 1; k <= nc; ++k) a += (*this)(i,k)*(*A)(k,j);
      (*B)(i,j) = a;
    }
  }
  return MT_SUCCESS;
});

csr_matrix::value_type csr_matrix::operator()(csr_matrix::index_type r, csr_matrix::index_type c) const noexcept
{
  for (auto row = rows_[r-1]-1; row < rows_[r]-1; ++row) {
    const auto col = cols_[row];

    if (col == c) return data_[row];
    else if (col > c) break; // not found
  }
  return value_type{};
}

csr_matrix::reference_type csr_matrix::operator()(csr_matrix::index_type r, csr_matrix::index_type c) noexcept
{
  auto pos = rows_[r-1]-1;
  auto col = 0;

  for (; pos < rows_[r]-1; ++pos) {
    col = cols_[pos];
    if (col >= c) break;
  }

  if (col != c) {
    data_.emplace(std::next(data_.cbegin(),pos));
    cols_.insert(std::next(cols_.cbegin(),pos),c);
    for (auto i = r; i <= nrows_; ++i) ++rows_[i];
  }
  return data_[pos];
}

mt_error_t csr_matrix::view(std::ostream& ostrm) const noexcept MT_TRY(
{
  for (auto i = 0; i < nrows_; ++i) {
    for (auto j = 0; j < ncols_-1; ++j) ostrm<<(*this)(i,j)<<',';
    ostrm<<(*this)(i,ncols_-1)<<'\n';
  }
  return MT_SUCCESS;
});

csr_matrix::index_type csr_matrix::nnz() const noexcept { return data_.size(); }

} // namespace mt
