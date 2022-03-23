#include <matrix_tuner/matrix/matrix.hpp>

int main(int argc, char *argv[])
{
  auto dmat = mt::dense_matrix(std::vector<double>{
      0,1,2,
      3,4,5,
      6,7,8
    });

  MTCHECK(dmat.view());

  auto cmat = mt::coo_matrix{dmat};
  cmat.view();
  return 0;
}
