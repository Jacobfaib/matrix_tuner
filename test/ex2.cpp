#include <matrix_tuner/matrix/coo.hpp>

#include <memory>

int main(int argc, char *argv[])
{
  auto coo = mt::coo_matrix{10,10};
  coo(1,1) = 10;
  MTCHECK(coo.view());
  return 0;
}
