#include <matrix_tuner/matrix/coo.hpp>

#include <algorithm> // std::lower_bound

namespace mt
{

mt_error_t coo_matrix::mult(const matrix*,matrix*) const noexcept
{
  return 0;
}


template <typename T>
static auto print_vector(std::vector<T>& v)
{
  for (auto &&d : v) std::cout<<d<<',';
  std::cout<<std::endl;
}

template <typename T>
static auto insert_sorted(std::vector<T>& v, const T& needle)
{
  // find the correct sorted insert position
  const auto cend = std::cend(v);
  const auto loc  = std::lower_bound(std::cbegin(v),cend,needle);

  // only insert if not a duplicate
  // test for end() first to make duplicate test safe
  return (loc == cend || *loc != needle) ? v.insert(loc,needle) : loc;
}

template <typename T, typename Idx_t = typename matrix<T>::index_type>
static auto operator_brackets(Idx_t r, Idx_t c, std::vector<Idx_t>& rows, std::vector<Idx_t>& cols, std::vector<T>& data, Idx_t max_idx) noexcept
{
  insert_sorted(rows,r); // unused
  const auto col_it = insert_sorted(cols,c);
  // can't do this in-line since iterator may be invalidated due to insert
  const auto idx = std::distance(cols.cbegin(),col_it);

  if (idx >= data.size()) data.resize(std::min(static_cast<decltype(max_idx)>(idx+1),max_idx));
  return idx;
}

coo_matrix::const_reference_type coo_matrix::operator()(coo_matrix::index_type r, coo_matrix::index_type c) const noexcept
{
  return data_[operator_brackets(r,c,rows_,cols_,data_,nrows_*ncols_)];
}

coo_matrix::reference_type coo_matrix::operator()(coo_matrix::index_type r, coo_matrix::index_type c) noexcept
{
  return data_[operator_brackets(r,c,rows_,cols_,data_,nrows_*ncols_)];
}

mt_error_t coo_matrix::view(std::ostream& ostrm) const noexcept
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
  return 0;
}

} // namespace mt
