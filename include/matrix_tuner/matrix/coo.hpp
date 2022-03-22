#ifndef MT_COO_HPP
#define MT_COO_HPP

#include <matrix_tuner/matrix/matrix.hpp>

#include <vector>

namespace mt
{

class MT_VISIBILITY_EXTERNAL coo_matrix;

class coo_matrix : public matrix<>
{
  MT_COMMON_MATRIX_HEADER;

  mt_error_t           mult(const matrix*,matrix*)       const noexcept final;
  const_reference_type operator()(index_type,index_type) const noexcept final;
  reference_type       operator()(index_type,index_type)       noexcept final;
  mt_error_t           view(std::ostream& = std::cout)   const noexcept final;

  virtual ~coo_matrix() noexcept { }

private:
  mutable std::vector<index_type> rows_;
  mutable std::vector<index_type> cols_;
  mutable std::vector<value_type> data_;
};

} // namespace mt


#endif // MT_COO_HPP