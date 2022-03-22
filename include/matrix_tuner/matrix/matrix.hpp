#ifndef MT_MATRIX_HPP
#define MT_MATRIX_HPP

#include <matrix_tuner/sys/sys.hpp>

namespace mt
{

class matrix
{
protected:
  std::size_t rows_;
  std::size_t cols_;

  constexpr matrix(std::size_t r = 0, std::size_t c = 0) noexcept : rows_(r), cols_(c) { }

  virtual ~matrix() { };
};

} // namespace mt

#endif // MATRIX_HPP
