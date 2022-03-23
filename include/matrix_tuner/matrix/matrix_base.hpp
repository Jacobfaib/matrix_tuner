#ifndef MT_MATRIX_MATRIX_BASE_HPP
#define MT_MATRIX_MATRIX_BASE_HPP

#include <matrix_tuner/sys/sys.hpp>

#include <type_traits>
#include <iostream>

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

  constexpr matrix(index_type r = 0, index_type c = 0) noexcept : nrows_(r), ncols_(c) { }

  virtual mt_error_t           mult(const matrix*,matrix*)       const noexcept = 0;
  virtual const_reference_type operator()(index_type,index_type) const noexcept = 0;
  virtual reference_type       operator()(index_type,index_type)       noexcept = 0;
  virtual mt_error_t           view(std::ostream& = std::cout)   const noexcept = 0;

  virtual ~matrix() { };

protected:
  index_type nrows_;
  index_type ncols_;
};

#define MT_COMMON_MATRIX_HEADER                 \
private:                                        \
  using base_type = matrix<>;                   \
public:                                         \
  using matrix::matrix



} // namespace mt

#endif // MT_MATRIX_MATRIX_BASE_HPP
