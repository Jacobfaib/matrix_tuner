#include <matrix_tuner/sys/sys.hpp>

#include <cstdarg> // va_list etc
#include <array>
#include <tuple>
#include <queue>
#include <string>
#include <iostream>

namespace mt
{

static constexpr auto error_strings = std::array<const char*,MT_ERROR_MAX>{
  "unknown error",
  "logic error",
  "index out of range",
  "incompatible sizes",
  "max error code"
};

static constexpr const char error_bar[] = "-----------------------------------------------------------------------------------";

class backtrace
{
private:
  using entry_type  = std::tuple<std::string,std::string,int>;
  using stack_type  = std::queue<entry_type>;
  using buffer_type = std::array<char,2048>;

  const mt_error_t initial_error_;
  buffer_type      buffer_;
  stack_type       stack_;

public:
  backtrace(mt_error_t errc) noexcept : initial_error_(errc) { }

  const entry_type& last() const noexcept { return stack_.back();  }

  mt_error_t error_code() const noexcept  { return initial_error_; }

  buffer_type& buffer() noexcept { return buffer_; }

  void push(std::string file, std::string func, int line) noexcept
  {
    stack_.push(std::make_tuple(std::move(file),std::move(func),line));
  }

  void unwind() noexcept
  {
    int i = 0;

    // std::endl all of these to flush stdout
    std::cerr<<"Matrix Tuner ERROR "<<error_bar<<std::endl;
    std::cerr<<"Error code:    "<<initial_error_<<std::endl;
    std::cerr<<"Error message: "<<buffer_.data()<<std::endl;
    while (!stack_.empty()) {
      int         line;
      std::string func,file;

      std::tie(file,func,line) = stack_.front();
      std::cerr<<"ERROR: ["<<i++<<"] "<<func<<" line "<<line<<" in file "<<file<<std::endl;
      stack_.pop();
    }
    std::cerr<<"Matrix Tuner ERROR "<<error_bar<<std::endl;
  }
};

mt_error_t error(const char *file, const char *func, int line, error_type, mt_error_t errc, const char *mess,...) noexcept
{
  static auto bt = backtrace{errc};

  bt.push(file,func,line);
  if (std::strlen(mess)) {
    auto argp = std::va_list{};
    auto& buf = bt.buffer();

    va_start(argp,mess);
    std::vsnprintf(buf.data(),buf.size(),mess,argp);
    va_end(argp);
  }
  if (std::get<1>(bt.last()).find("main(") != std::string::npos) {
    bt.unwind();
    exit(bt.error_code()); // noreturn
    __builtin_unreachable();
  }
  return errc;
}

} // namespace mt
