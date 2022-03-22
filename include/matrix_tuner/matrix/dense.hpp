#ifndef MT_DENSE_HPP
#define MT_DENSE_HPP

#include <matrix_tuner/matrix/matrix.hpp>

#include <vector>

namespace mt
{

class dense_matrix : matrix
{
  using base_type = matrix;

public:
  using base_type::base_type;

  error_code mult(const matrix*,matrix*) const noexcept;

private:
  std::vector<double> data_;
};

} // namespace mt

#endif // MT_DENSE_HPP
