#ifndef MT_MATRIX_CSR_HPP
#define MT_MATRIX_CSR_HPP

#include <matrix_tuner/matrix/matrix_base.hpp>

#include <vector>

namespace mt
{

class MT_VISIBILITY_EXTERNAL csr_matrix;

class csr_matrix : public matrix<>
{
  MT_COMMON_MATRIX_HEADER;

  csr_matrix(const matrix&) noexcept;

  mt_error_t     mult(const matrix*,matrix*)       const noexcept final;
  value_type     operator()(index_type,index_type) const noexcept final;
  reference_type operator()(index_type,index_type)       noexcept final;
  mt_error_t     view(std::ostream& = std::cout)   const noexcept final;
  index_type     nnz()                             const noexcept final;

  virtual ~csr_matrix() noexcept { }

private:
  mutable std::vector<index_type> rows_;
  mutable std::vector<index_type> cols_;
  mutable std::vector<value_type> data_;
};

} // namespace mt

#endif // MT_MATRIX_CSR_HPP
