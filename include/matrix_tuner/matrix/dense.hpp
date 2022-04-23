#ifndef MT_MATRIX_DENSE_HPP
#define MT_MATRIX_DENSE_HPP

#include <matrix_tuner/matrix/matrix_base.hpp>

#include <vector> // std::vector
#include <cmath>  // std::sqrt

namespace mt
{

class MT_VISIBILITY_EXTERNAL dense_matrix;

class dense_matrix : public matrix<>
{
  MT_COMMON_MATRIX_HEADER;

  dense_matrix(const matrix&) noexcept;

  dense_matrix(std::vector<value_type>&& d, index_type r = -1, index_type c = -1) noexcept
    : base_type{r,c}, data_(std::move(d))
  {
    const auto size = data_.size();
    if (r == -1 && c == -1) nrows_ = ncols_ = std::sqrt(size);
    else if (r == -1)       nrows_ = size/c;
    else if (c == -1)       ncols_ = size/r;
  }

  mt_error_t     mult(const matrix*,matrix*)             const noexcept final;
  mt_error_t     mult(const dense_matrix*,dense_matrix*) const noexcept;
  value_type     operator()(index_type,index_type)       const noexcept final;
  reference_type operator()(index_type,index_type)             noexcept final;
  mt_error_t     view(std::ostream& = std::cout)         const noexcept final;
  index_type     nnz()                                   const noexcept final;

  virtual ~dense_matrix() noexcept { }

private:
  std::vector<value_type> data_;
};

} // namespace mt

#endif // MT_MATRIX_DENSE_HPP
