#include <matrix_tuner/sys/device.hpp>

namespace mt
{

mt_error_t cuda_device::initialize() noexcept
{
  if (initialized_) return MT_SUCCESS;
  if (!handle_) {
    constexpr auto nit = 3;
    cublasStatus_t cberr;

    for (auto i = 0; i < nit; ++i) {
      cberr = cublasCreate(&handle_);
      if (mtlikely(cberr == CUBLAS_STATUS_SUCCESS)) break;
      if (mtunlikely(cberr != CUBLAS_STATUS_ALLOC_FAILED) && (cberr != CUBLAS_STATUS_NOT_INITIALIZED)) MTCHECK(cberr);
      if (i != nit-1) MTCHECK(sleep(3));
    }
    MTASSERT(cberr == CUBLAS_STATUS_SUCCESS,MT_ERROR_GPU,"Unable to initialize cublas");
  }
  if (!stream_) {
    MTCHECK(cudaStreamCreate(&stream_));
    MTCHECK(cublasSetStream(handle_,stream_));
  }
  initialized_ = true;
  return MT_SUCCESS;
}

mt_error_t cuda_device::finalize() noexcept
{
  if (handle_) {
    MTCHECK(cublasDestroy(handle_));
    handle_ = nullptr;
  }
  if (stream_) {
    MTCHECK(cudaStreamSynchronize(stream_));
    MTCHECK(cudaStreamDestroy(stream_));
    stream_ = nullptr;
  }
  initialized_ = false;
  return MT_SUCCESS;
}

} // namespace mt
