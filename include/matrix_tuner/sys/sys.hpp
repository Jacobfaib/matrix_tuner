#ifndef MT_SYS_HPP
#define MT_SYS_HPP

#include <cstddef>

namespace mt
{

#if __cpluslpus >= 201703L // c++17
#  define MT_NODISCARD [[nodiscard]]
#else
#  define MT_NODISCARD
#endif

#ifdef __GNUC__
#  define MT_FUNCTION_NAME __PRETTY_FUNCTION__
#else
#  define MT_FUNCTION_NAME __func__
#endif

#ifndef __has_attribute
#  define __has_attribute(x) 0
#endif

#define MT_HAS_ATTRIBUTE(attr) __has_attribute(attr)
#define MT_ATTRIBUTE(attr) __attribute__((attr))

#if MT_HAS_ATTRIBUTE(visibility)
#  define MT_VISIBILITY_EXTERNAL MT_ATTRIBUTE(visibility ("default"))
#  define MT_VISIBILITY_INTERNAL MT_ATTRIBUTE(visibility ("hidden"))
#else
#  define MT_VISIBILITY_INTERNAL
#  define MT_VISIBILITY_EXTERNAL
#endif

#define	MT_INTERN extern MT_NODISCARD MT_VISIBILITY_INTERNAL
#define MT_EXTERN extern MT_NODISCARD MT_VISIBILITY_EXTERNAL

#define mtunlikely(...) __builtin_expect(!!(__VA_ARGS__),0)
#define mtlikely(...)   __builtin_expect(!!(__VA_ARGS__),1)

// declare the enum as C-style so we don't get annoying compile warnings about conversions
using mt_error_t = int;
#define MT_SUCCESS            0
#define MT_ERROR_UNKNOWN      1
#define MT_ERROR_LOGIC        2
#define MT_ERROR_OUT_OF_RANGE 3
#define MT_ERROR_INCOMP_SIZE  4
#define MT_ERROR_GPU          5
#define MT_ERROR_MAX          6

namespace detail
{

static inline constexpr bool check_error(mt_error_t err) noexcept { return err != MT_SUCCESS; }
static inline constexpr mt_error_t error_convert(mt_error_t err) noexcept { return err; }

}

enum class error_type
{
  ERROR_INITIAL,
  ERROR_REPEAT
};

#define MTCHECK(...) do {                                                                      \
    const auto errc = __VA_ARGS__;                                                             \
    if (mtunlikely(mt::detail::check_error(errc))) {                                           \
      return mt::error(__FILE__,MT_FUNCTION_NAME,__LINE__,mt::error_type::ERROR_REPEAT,mt::detail::error_convert(errc),nullptr); \
    }                                                                                          \
  } while (0)

#define MTCHECK_ABORT(...) do {                 \
    const auto errc = __VA_ARGS__;              \
    if (mtunlikely(mt::detail::check_error(errc))) {    \
                                                        \
    }                                                   \
  } while (0)

#define MTASSERT(cond,...) do {                         \
    if (mtunlikely(!(cond))) MTSETERR(__VA_ARGS__);     \
  } while (0)

#define MTSETERR(errcode,...) return mt::error(__FILE__,MT_FUNCTION_NAME,__LINE__,mt::error_type::ERROR_INITIAL,errcode,__VA_ARGS__)

#define MT_CONVERT_CATCH(ex_type,error_code)                    \
  catch (const ex_type & ex) { MTSETERR(error_code,ex.what()); }

#define MT_TRY(...) try { __VA_ARGS__; }                                        \
  MT_CONVERT_CATCH(std::logic_error,MT_ERROR_LOGIC)                             \
  MT_CONVERT_CATCH(std::exception,MT_ERROR_UNKNOWN)                             \
  catch (...) { MTSETERR(MT_ERROR_UNKNOWN,"Unknown exception caught");  }

MT_EXTERN mt_error_t error(const char*,const char*,int,error_type,mt_error_t,const char*,...) noexcept;
MT_EXTERN mt_error_t sleep(std::size_t) noexcept;

enum class device_type
{
  host,
  cuda
};

} // namespace mt

#endif // MT_SYS_HPP
