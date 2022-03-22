#ifndef MT_DENSE_HPP
#define MT_DENSE_HPP

#include <matrix_tuner/matrix/matrix.hpp>

#include <vector>

namespace mt
{

class MT_VISIBILITY_EXTERNAL dense_matrix;

class dense_matrix : public matrix<>
{
  MT_COMMON_MATRIX_HEADER;

  mt_error_t mult(const matrix*,matrix*) const noexcept final;

  virtual ~dense_matrix() noexcept { }

private:
  std::vector<double> data_;
};

} // namespace mt

#endif // MT_DENSE_HPP
