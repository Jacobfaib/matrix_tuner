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

namespace detail
{

template <typename T> static inline constexpr bool check_error(T&& err) noexcept { return !!err; }

}

enum class error_type
{
  ERROR_INITIAL,
  ERROR_REPEAT
};

#define MTCHECK(...) do {                                                                      \
    const auto errc = __VA_ARGS__;                                                             \
    if (mtunlikely(mt::detail::check_error(errc))) {                                           \
      return mt::error_handler(__FILE__,__func__,__LINE__,errc,mt::error_type::ERROR_REPEAT);  \
    }                                                                                          \
  } while (0)

#define MTSETERR(errcode,...) return mt::error(__FILE__,SF_FUNCTION_NAME,__LINE__,mt::error_type::ERROR_INITIAL,errcode,__VA_ARGS__)

using mt_error_t = int;

MT_EXTERN mt_error_t error_handler(const char*,const char*,int,mt_error_t,error_type);
template <typename T>
MT_EXTERN mt_error_t error(const char*,const char*,int,error_type,mt_error_t,const char*,T&&...);

} // namespace mt

#endif // MT_SYS_HPP
