#include <matrix_tuner/matrix/dense.hpp>

namespace mt
{

dense_matrix::dense_matrix(const matrix &other) noexcept : base_type{other.nrows(),other.ncols()}
{
  data_.reserve(nrows_*ncols_);
  base_type::deep_copy(other);
}

mt_error_t dense_matrix::mult(const matrix *A, matrix *B) const noexcept MT_TRY(
{
  return MT_SUCCESS;
});

mt_error_t dense_matrix::mult(const dense_matrix *A, dense_matrix *B) const noexcept MT_TRY(
{
  MTCHECK(duplicate_(A,&B));
  if (device_->type() == device_type::host) {
    const auto nc = nrows(), anc = A->ncols(), anr = A->nrows();

    for (auto i = index_type{0}; i < nc; ++i) {
      for (auto j = index_type{0}; j < anc; ++j) {
        auto tmp = value_type{0};
        for (auto k = index_type{0}; k < anr; ++k) tmp += (*this)(i,k)*(*A)(k,j);
        (*B)(i,j) = tmp;
      }
    }
  } else {
    constexpr auto  one    = 1.0, zero = 0.0;
    const auto      size   = data_.size()*sizeof(value_type);
    auto            device = dynamic_cast<cuda_device*>(device_);
    value_type     *d_buf_s,*d_buf_a,*d_buf_b;

    B->data_.resize(data_.size());
    MTCHECK(cudaMalloc(reinterpret_cast<void**>(&d_buf_s),size));
    MTCHECK(cudaMalloc(reinterpret_cast<void**>(&d_buf_a),size));
    MTCHECK(cudaMalloc(reinterpret_cast<void**>(&d_buf_b),size));
    MTCHECK(cudaMemcpyAsync(d_buf_s,data_.data(),size,cudaMemcpyDefault,device->stream()));
    MTCHECK(cudaMemcpyAsync(d_buf_a,A->data_.data(),size,cudaMemcpyDefault,device->stream()));
    MTCHECK(cublasDgemm(device->handle(),CUBLAS_OP_N, CUBLAS_OP_N,nrows(),ncols(),ncols(),&one,d_buf_s,ncols(),d_buf_a,nrows(),&zero,d_buf_b,nrows()));
    MTCHECK(cudaMemcpyAsync(B->data_.data(),d_buf_b,size,cudaMemcpyDefault,device->stream()));
    MTCHECK(cudaFree(d_buf_s));
    MTCHECK(cudaFree(d_buf_a));
    MTCHECK(cudaFree(d_buf_b));
  }
  return MT_SUCCESS;
});

dense_matrix::value_type dense_matrix::operator()(dense_matrix::index_type r, dense_matrix::index_type c) const noexcept
{
  return data_[(r*ncols_)+c];
}

dense_matrix::reference_type dense_matrix::operator()(dense_matrix::index_type r, dense_matrix::index_type c) noexcept
{
  return data_[(r*ncols_)+c];
}

mt_error_t dense_matrix::view(std::ostream& ostrm) const noexcept MT_TRY(
{
  for (auto i = 0; i < nrows_; ++i) {
    for (auto j = 0; j < ncols_-1; ++j) ostrm<<(*this)(i,j)<<',';
    ostrm<<(*this)(i,ncols_-1)<<'\n';
  }
  return MT_SUCCESS;
});

dense_matrix::index_type dense_matrix::nnz() const noexcept { return data_.size(); }

} // namespace mt
