#include <gtest/gtest.h>
#include <hpc_matrix3x3.hpp>
#include <iostream>

using Real = double;
using Tensor = hpc::matrix3x3<Real>;

template <typename T>
std::ostream &
operator<<(std::ostream & os, hpc::matrix3x3<T> const & A)
{
  os << std::scientific << std::setprecision(17);
  for (auto i = 0; i < 3; ++i) {
    os << std::setw(24) << A(i,0);
    for (auto j = 1; j < 3; ++j) {
      os << "," << std::setw(24) << A(i,j);
    }
    os << std::endl;
  }
  return os;
}

int
main(int ac, char* av[])
{
  ::testing::GTEST_FLAG(print_time) = true;
  ::testing::InitGoogleTest(&ac, av);
  auto const retval = RUN_ALL_TESTS();
  return retval;
}

TEST(tensor, log)
{
  // Identity
  auto const eps = hpc::machine_epsilon<Real>();
  auto const I = Tensor::identity();
  auto const i = hpc::log(I);
  auto const error_I = hpc::norm(i) / hpc::norm(I);
  ASSERT_LE(error_I, eps);
  // 1/8 of a rotation
  auto const tau = 2.0 * std::acos(-1.0);
  auto const c = std::sqrt(2.0) / 2.0;
  auto const R = Tensor(c, -c, 0.0, c, c, 0.0, 0.0, 0.0, 1.0);
  auto const r = log(R);
  auto const error_R = std::abs(r(0,1) + tau / 8.0);
  ASSERT_LE(error_R, eps);
  auto const error_r = std::abs(r(0,1) + r(1,0));
  ASSERT_LE(error_r, eps);
}

TEST(tensor, inverse)
{
  auto const eps = hpc::machine_epsilon<Real>();
  // Tolerance: see Golub & Van Loan, Matrix Computations 4th Ed., pp 122-123
  auto const dim = 3;
  auto const tol = 2 * (dim - 1) * eps;
  Tensor const A(7, 1, 2, 3, 8, 4, 5, 6, 9);
  auto const B = hpc::inverse_full_pivot(A);
  auto const I = Tensor::identity();
  auto const C = A * B;
  auto const error_1 = hpc::norm(C - I) / hpc::norm(A);
  ASSERT_LE(error_1, tol);
  auto const D = B * A;
  auto const error_2 = hpc::norm(D - I) / hpc::norm(A);
  ASSERT_LE(error_2, tol);
}

TEST(tensor, sqrt)
{
  auto const eps = hpc::machine_epsilon<Real>();
  // Tolerance: see Golub & Van Loan, Matrix Computations 4th Ed., pp 122-123
  auto const dim = 3;
  auto const tol = 2 * (dim - 1) * eps;
  Tensor const A(7, 1, 2, 3, 8, 4, 5, 6, 9);
  auto const B = hpc::sqrt(A);
  auto const C = B * B;
  auto const error = hpc::norm(C - A) / hpc::norm(A);
  ASSERT_LE(error, tol);
}