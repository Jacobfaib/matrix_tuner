#ifndef MT_MATRIX_MATRIX_BASE_HPP
#define MT_MATRIX_MATRIX_BASE_HPP

#include <matrix_tuner/sys/sys.hpp>
#include <matrix_tuner/sys/device.hpp>

#include <utility>  // std::make_pair
#include <iostream> // std::cout

namespace mt
{

template <typename T = double> class MT_VISIBILITY_EXTERNAL matrix;

template <typename T>
class matrix
{
public:
  using value_type           = T;
  using index_type           = std::size_t;
  using reference_type       = value_type&;
  using const_reference_type = const value_type&;

  constexpr matrix(index_type r = 0, index_type c = 0) noexcept
    : nrows_(r), ncols_(c), device_(new host_device{})
  { }

  virtual mt_error_t     mult(const matrix*,matrix*)       const noexcept = 0;
  virtual value_type     operator()(index_type,index_type) const noexcept = 0;
  virtual reference_type operator()(index_type,index_type)       noexcept = 0;
  virtual mt_error_t     view(std::ostream& = std::cout)   const noexcept = 0;
  virtual index_type     nnz()                             const noexcept = 0;

  virtual ~matrix() { if (device_) delete device_; };

  auto nrows() const noexcept { return nrows_; }
  auto ncols() const noexcept { return ncols_; }
  auto shape() const noexcept { return std::make_pair(nrows(),ncols()); }

protected:
  index_type  nrows_;
  index_type  ncols_;
  device     *device_;

  mt_error_t deep_copy(const matrix &other) noexcept
  {
    for (auto i = 0; i < nrows_; ++i) {
      for (auto j = 0; j < ncols_; ++j) if (const auto val = other(i,j)) this->operator()(i,j) = val;
    }
    return MT_SUCCESS;
  }

  mt_error_t check_compatible_(const matrix& other) const noexcept MT_TRY(
  {
    if (mtunlikely(ncols() != other.nrows())) MTSETERR(MT_ERROR_INCOMP_SIZE,"Dimensions mismatch (%zu,%zu) not compatible with (%zu,%zu)",nrows(),ncols(),other.nrows(),other.ncols());
    return MT_SUCCESS;
  });

  template <typename M>
  mt_error_t duplicate_(const matrix *ref_right, M **ret) const noexcept MT_TRY(
  {
    MTCHECK(check_compatible_(*ref_right));
    if (*ret) {
      MTCHECK(check_compatible_(**ret));
    } else {
      *ret = new M{nrows(),ref_right->ncols()};
    }
    return MT_SUCCESS;
  });
};

#define MT_COMMON_MATRIX_HEADER                 \
private:                                        \
  using base_type = matrix<>;                   \
public:                                         \
  using matrix::matrix

} // namespace mt

#endif // MT_MATRIX_MATRIX_BASE_HPP
