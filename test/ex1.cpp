#include <matrix_tuner/matrix/matrix.hpp>

int main(int argc, char *argv[])
{
  auto dense = mt::dense_matrix(10,10);
  auto coo   = mt::coo_matrix(15,15);
  return 0;
}
