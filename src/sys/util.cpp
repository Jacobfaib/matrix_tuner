#include <matrix_tuner/sys/sys.hpp>

#include <chrono>
#include <thread>

namespace mt
{

mt_error_t sleep(std::size_t s) noexcept MT_TRY(
{
  std::this_thread::sleep_for(std::chrono::seconds(s));
  return MT_SUCCESS;
});

} // namespace mt
