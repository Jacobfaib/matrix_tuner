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
  MTCHECK(duplicate_(A,&B));

  const auto nr  = nrows();
  auto       marker = std::vector<int>(A->ncols(),-1);
  index_type nnz    = 0;

  B->rows_.resize(nr+1);
  B->rows_[0] = nnz;

  // preallocate
  for (index_type r = 0; r < nr; ++r) {
    for (auto rmine = rows_[r]; rmine < rows_[r+1]; ++rmine) {
      const auto cmine = cols_[rmine];

      for (auto rother = A->rows_[cmine]; rother < A->rows_[cmine+1]; ++rother) {
        auto& mark = marker[A->cols_[rother]];

        if (mark != r) {
          mark = r;
          ++nnz;
        }
      }
    }
    B->rows_[r+1] = nnz;
  }
  B->cols_.resize(nnz);
  B->data_.resize(nnz);

  std::fill(marker.begin(),marker.end(),-1);

  // do the mult
  for (index_type r = 0, counter = 0; r < nr; ++r) {
    const auto rstart = counter;

    for (auto rmine = rows_[r]; rmine < rows_[r+1]; ++rmine) {
      const auto cmine = cols_[rmine];
      const auto dmine = data_[rmine];

      for (auto rother = A->rows_[cmine]; rother < A->rows_[cmine+1]; ++rother) {
        const auto cother = A->cols_[rother];
        const auto dother = A->data_[rother];
        const auto val    = dmine*dother;
        auto&      mark   = marker[cother];

        if (mark < rstart) {
          B->cols_[counter] = cother;
          B->data_[counter] = val;
          mark = counter++;
        } else {
          B->data_[mark] += val;
        }
      }
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
