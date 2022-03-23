#include <matrix_tuner/matrix/coo.hpp>

#include <algorithm> // std::lower_bound

namespace mt
{

coo_matrix::coo_matrix(const matrix &other) noexcept : base_type{other.nrows(),other.ncols()}
{
  base_type::deep_copy(other);
  for (auto x : data_) std::cout<<x<<',';
}

mt_error_t coo_matrix::mult(const matrix*,matrix*) const noexcept MT_TRY(
{
  return MT_SUCCESS;
});

template <typename T>
static auto contains(std::vector<T>& v, const T& needle) noexcept
{
  const auto cend = v.cend();
  return std::lower_bound(v.cbegin(),cend,needle) != cend;
}

template <bool insert, typename T>
static auto insert_sorted(std::vector<T>& v, const T& needle) noexcept
{
  // find the correct sorted insert position
  const auto cend = v.cend();
  const auto loc  = std::lower_bound(v.cbegin(),cend,needle);

  // only insert if not a duplicate
  // test for end() first to make duplicate test safe
  return (insert && (loc == cend || *loc != needle)) ? v.insert(loc,needle) : loc;
}

template < bool insert, typename T, typename Idx_t = typename matrix<T>::index_type>
static auto operator_brackets(Idx_t r, Idx_t c, std::vector<Idx_t>& rows, std::vector<Idx_t>& cols, std::vector<T>& data, Idx_t max_idx) noexcept
{
  insert_sorted<insert>(rows,r); // unused
  const auto col_it = insert_sorted<insert>(cols,c);
  // can't do this in-line since iterator may be invalidated due to insert
  const auto idx = std::distance(cols.cbegin(),col_it);

  if (insert && (idx >= data.size())) data.resize(std::min(static_cast<Idx_t>(idx+1),max_idx));
  return idx;
}

coo_matrix::const_reference_type coo_matrix::operator()(coo_matrix::index_type r, coo_matrix::index_type c) const noexcept
{
  if (contains(rows_,r) && contains(cols_,c)) {
    return data_[operator_brackets<false>(r,c,rows_,cols_,data_,nrows_*ncols_)];
  } else {
    static auto zero = value_type{0};

    return zero;
  }
}

coo_matrix::reference_type coo_matrix::operator()(coo_matrix::index_type r, coo_matrix::index_type c) noexcept
{
  return data_[operator_brackets<true>(r,c,rows_,cols_,data_,nrows_*ncols_)];
}

mt_error_t coo_matrix::view(std::ostream& ostrm) const noexcept MT_TRY(
{
  const auto rend   = rows_.cend();
  auto       rbegin = rows_.cbegin();
  auto       cbegin = cols_.cbegin();
  auto       vbegin = data_.cbegin();

  const auto get_value = [&](auto idx, auto jdx)
  {
    // use rows as canary
    if ((rbegin != rend && *rbegin == idx) && *cbegin == jdx) {
      ++rbegin;++cbegin;
      return *vbegin++;
    } else {
      return value_type{};
    }
  };

  for (auto i = 0; i < nrows_; ++i) {
    for (auto j = 0; j < ncols_-1; ++j) ostrm<<get_value(i,j)<<',';
    ostrm<<get_value(i,ncols_-1)<<'\n';
  }
  return MT_SUCCESS;
});

coo_matrix::index_type coo_matrix::nnz() const noexcept { return data_.size(); }

// CONVERSION to/from CSR:
// https://stackoverflow.com/questions/23583975/convert-coo-to-csr-format-in-c

} // namespace mt
