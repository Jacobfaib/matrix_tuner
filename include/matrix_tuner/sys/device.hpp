#ifndef MT_SYS_DEVICE_HPP
#define MT_SYS_DEVICE_HPP

#include <matrix_tuner/sys/sys.hpp>

#ifdef MT_HAVE_DEVICE
#  include <cuda_runtime.h>
#  include <cublas_v2.h>
#else
enum cudaError_t
{
  cudaSuccess
};
struct opaque_stream { };
using cudaStream_t = opaque_stream*;
static inline cudaError_t cudaStreamCreate(cudaStream_t*) { return cudaSuccess; }
static inline cudaError_t cudaStreamSynchronize(cudaStream_t) { return cudaSuccess; }
static inline cudaError_t cudaStreamDestroy(cudaStream_t) { return cudaSuccess; }

enum cublasStatus_t
{
  CUBLAS_STATUS_SUCCESS,
  CUBLAS_STATUS_ALLOC_FAILED,
  CUBLAS_STATUS_NOT_INITIALIZED
};

struct opaque_handle { };
using cublasHandle_t = opaque_handle*;
static inline cublasStatus_t cublasCreate(cublasHandle_t *handle) { return CUBLAS_STATUS_SUCCESS; }
static inline cublasStatus_t cublasSetStream(cublasHandle_t,cudaStream_t) { return CUBLAS_STATUS_SUCCESS; }
static inline cublasStatus_t cublasDestroy(cublasHandle_t) { return CUBLAS_STATUS_SUCCESS; }
#endif // MT_HAVE_DEVICE

namespace mt
{

namespace detail
{

static inline constexpr bool check_error(cudaError_t err) noexcept { return err != cudaSuccess; }
static inline constexpr mt_error_t error_convert(cudaError_t err) noexcept { return MT_ERROR_GPU; }

static inline constexpr bool check_error(cublasStatus_t err) noexcept { return err != CUBLAS_STATUS_SUCCESS; }
static inline constexpr mt_error_t error_convert(cublasStatus_t err) noexcept { return MT_ERROR_GPU; }

} // namespace detail

class MT_VISIBILITY_EXTERNAL device;
class MT_VISIBILITY_EXTERNAL host_device;
class MT_VISIBILITY_EXTERNAL cuda_device;

class device
{
protected:
  bool initialized_;
  const device_type type_;

public:
  constexpr device(device_type d, bool init = false) noexcept : initialized_(init), type_(d) { }
  virtual ~device() noexcept { }
  virtual mt_error_t initialize()    noexcept = 0;
  virtual mt_error_t finalize()      noexcept = 0;

  constexpr bool initialized() const noexcept { return initialized_; }
  constexpr device_type type() const noexcept { return type_;        }
};

class host_device : public device
{
public:
  /* the host is always initialized */
  constexpr host_device() noexcept : device(device_type::host,true) { }

  virtual ~host_device() noexcept { }

  constexpr mt_error_t initialize() const noexcept { return MT_SUCCESS; }
  constexpr mt_error_t finalize()   const noexcept { return MT_SUCCESS; }


  mt_error_t initialize() noexcept final { return const_cast<const host_device*>(this)->initialize(); }
  mt_error_t finalize()   noexcept final { return const_cast<const host_device*>(this)->finalize();   }
};

class cuda_device : public device
{
  cudaStream_t   stream_ = nullptr;
  cublasHandle_t handle_ = nullptr;

public:
  constexpr cuda_device() noexcept : device(device_type::cuda) { }

  virtual ~cuda_device() noexcept { MTCHECK_ABORT(finalize()); }

  mt_error_t initialize() noexcept final;
  mt_error_t finalize()   noexcept final;
};

} // namespace mt

#endif // MT_SYS_DEVICE_HPP
