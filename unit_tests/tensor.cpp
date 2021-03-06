#include <gtest/gtest.h>

#include <hpc_matrix3x3.hpp>
#include <otm_util.hpp>

using Real   = double;
using Tensor = hpc::matrix3x3<Real>;
using Vector = hpc::vector3<Real>;

TEST(tensor, exp)
{
  auto const eps        = hpc::machine_epsilon<Real>();
  auto const A          = Tensor(2.5, 0.5, 1, 0.5, 2.5, 1, 1, 1, 2);
  auto const a          = sqrt(2.0) / 2.0;
  auto const b          = sqrt(3.0) / 3.0;
  auto const c          = sqrt(6.0) / 6.0;
  auto const V          = Tensor(c, a, b, c, -a, b, -2 * c, 0, b);
  auto const p          = std::exp(1.0);
  auto const q          = std::exp(2.0);
  auto const r          = std::exp(4.0);
  auto const D          = Tensor(p, 0, 0, 0, q, 0, 0, 0, r);
  auto const B          = hpc::exp(A);
  auto const C          = V * D * hpc::transpose(V);
  auto const F          = hpc::exp_taylor(A);
  auto const error_pade = hpc::norm(B - C) / hpc::norm(A);
  ASSERT_LE(error_pade, 9 * eps);
  auto const error_taylor = hpc::norm(F - C) / hpc::norm(A);
  ASSERT_LE(error_taylor, 20 * eps);
}

TEST(tensor, log)
{
  // Identity
  auto const eps     = hpc::machine_epsilon<Real>();
  auto const I       = Tensor::identity();
  auto const i       = hpc::log(I);
  auto const error_I = hpc::norm(i) / hpc::norm(I);
  ASSERT_LE(error_I, eps);
  // 1/8 of a rotation
  auto const tau     = 2.0 * std::acos(-1.0);
  auto const c       = std::sqrt(2.0) / 2.0;
  auto const R       = Tensor(c, -c, 0.0, c, c, 0.0, 0.0, 0.0, 1.0);
  auto const r       = log(R);
  auto const error_R = std::abs(r(0, 1) + tau / 8.0);
  ASSERT_LE(error_R, eps);
  auto const error_r = std::abs(r(0, 1) + r(1, 0));
  ASSERT_LE(error_r, eps);
  auto const A       = Tensor(7, 1, 2, 3, 8, 4, 5, 6, 9);
  auto const a       = hpc::log(A);
  auto const b       = hpc::log_gregory(A);
  auto const error_a = norm(b - a);
  auto const tol     = 32 * eps;
  ASSERT_LE(error_a, tol);
}

TEST(tensor, inverse)
{
  auto const eps = hpc::machine_epsilon<Real>();
  // Tolerance: see Golub & Van Loan, Matrix Computations 4th Ed., pp 122-123
  auto const dim     = 3;
  auto const tol     = 2 * (dim - 1) * eps;
  auto const A       = Tensor(7, 1, 2, 3, 8, 4, 5, 6, 9);
  auto const B       = hpc::inverse_full_pivot(A);
  auto const I       = Tensor::identity();
  auto const C       = A * B;
  auto const error_1 = hpc::norm(C - I) / hpc::norm(A);
  ASSERT_LE(error_1, tol);
  auto const D       = B * A;
  auto const error_2 = hpc::norm(D - I) / hpc::norm(A);
  ASSERT_LE(error_2, tol);
}

TEST(tensor, solve)
{
  auto const eps = hpc::machine_epsilon<Real>();
  // Tolerance: see Golub & Van Loan, Matrix Computations 4th Ed., pp 122-123
  auto const dim   = 3;
  auto const tol   = 2 * (dim - 1) * eps;
  auto const A     = Tensor(7, 1, 2, 3, 8, 4, 5, 6, 9);
  auto const b     = Vector(1, 2, 4);
  auto const c     = hpc::solve_full_pivot(A, b);
  auto const error = hpc::norm(A * c - b) / hpc::norm(A);
  ASSERT_LE(error, tol);
}

TEST(tensor, sqrt)
{
  auto const eps = hpc::machine_epsilon<Real>();
  // Tolerance: see Golub & Van Loan, Matrix Computations 4th Ed., pp 122-123
  auto const dim   = 3;
  auto const tol   = 2 * (dim - 1) * eps;
  auto const A     = Tensor(7, 1, 2, 3, 8, 4, 5, 6, 9);
  auto const B     = hpc::sqrt(A);
  auto const C     = B * B;
  auto const error = hpc::norm(C - A) / hpc::norm(A);
  ASSERT_LE(error, tol);
}

TEST(tensor, polar)
{
  auto const eps   = hpc::machine_epsilon<Real>();
  auto const c     = std::sqrt(2.0) / 2.0;
  auto const A     = Tensor(c, -c, 0.0, c, c, 0.0, 0.0, 0.0, 1.0);
  auto const B     = Tensor(2, 1, 0, 1, 2, 1, 0, 1, 2);
  auto const C     = A * B;
  auto       R     = 0.0 * A;
  auto       U     = 0.0 * B;
  std::tie(R, U)   = hpc::polar_right(C);
  auto const error = (hpc::norm(R - A) + hpc::norm(B - U)) / hpc::norm(C);
  ASSERT_LE(error, eps);
}
