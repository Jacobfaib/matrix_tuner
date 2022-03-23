#ifndef MT_MATRIX_DENSE_HPP
#define MT_MATRIX_DENSE_HPP

#include <matrix_tuner/matrix/matrix_base.hpp>

#include <vector>

namespace mt
{

class MT_VISIBILITY_EXTERNAL dense_matrix;

class dense_matrix : public matrix<>
{
  MT_COMMON_MATRIX_HEADER;

  mt_error_t           mult(const matrix*,matrix*)       const noexcept final;
  const_reference_type operator()(index_type,index_type) const noexcept final;
  reference_type       operator()(index_type,index_type)       noexcept final;
  mt_error_t           view(std::ostream& = std::cout)   const noexcept final;

  virtual ~dense_matrix() noexcept { }

private:
  std::vector<double> data_;
};

} // namespace mt

#endif // MT_MATRIX_DENSE_HPP
