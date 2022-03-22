#include <matrix_tuner/sys/sys.hpp>

#include <tuple>
#include <queue>
#include <string>
#include <iostream>

namespace mt
{

static constexpr const char error_bar[] = "-----------------------------------------------------------------------------------";

class backtrace
{
private:
  using entry_type = std::tuple<std::string,std::string,int>;
  using stack_type = std::queue<entry_type>;

  const mt_error_t initial_error_;
  stack_type       stack_;

public:
  backtrace(mt_error_t errc) noexcept : initial_error_(errc) { }

  const entry_type& last() const noexcept { return stack_.back();  }

  mt_error_t error_code() const noexcept  { return initial_error_; }

  void push(std::string file, std::string func, int line) noexcept
  {
    stack_.push(std::make_tuple(std::move(file),std::move(func),line));
  }

  void unwind() noexcept
  {
    int i = 0;

    // std::endl all of these to flush stdout
    std::cout<<"Matrix Tuner error "<<error_bar<<std::endl;
    std::cout<<"Error code "<<initial_error_<<std::endl;
    while (!stack_.empty()) {
      int         line;
      std::string func,file;

      std::tie(file,func,line) = stack_.front();
      std::cout<<"ERROR: ["<<i++<<"] "<<func<<"() line "<<line<<" in file "<<file<<std::endl;
      stack_.pop();
    }
    std::cout<<"Matrix Tuner error "<<error_bar<<std::endl;
  }
};

mt_error_t error_handler(const char *file, const char *func, int line, mt_error_t errc, error_type)
{
  static auto bt = backtrace{errc};

  bt.push(file,func,line);
  if (std::get<1>(bt.last()) == "main") {
    bt.unwind();
    exit(bt.error_code()); // noreturn
    __builtin_unreachable();
  }
  return errc;
}

} // namespace mt
