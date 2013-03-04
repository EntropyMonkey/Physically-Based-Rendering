// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#include <cmath>
#include <valarray>
#include "legendre.h"

using namespace std;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace
{
  const double M_4PI = 4.0*M_PI;

  const double fact[33] = { 1.0,
                            1.0,
                            2.0,
                            6.0,
                            24.0,
                            120.0,
                            720.0,
                            5040.0,
                            40320.0,
                            362880.0,
                            3628800.0,
                            39916800.0,
                            479001600.0,
                            6227020800.0,
                            87178291200.0,
                            1307674368000.0,
                            20922789888000.0,
                            355687428096000.0,
                            6402373705728000.0,
                            121645100408832000.0,
                            2432902008176640000.0,
                            51090942171709440000.0,
                            1124000727777607680000.0,
                            25852016738884976640000.0,
                            620448401733239439360000.0,
                            15511210043330985984000000.0,
                            403291461126605635584000000.0,
                            10888869450418352160768000000.0,
                            304888344611713860501504000000.0,
                            8841761993739701954543616000000.0,
                            265252859812191058636308480000000.0,
                            8222838654177922817725562880000000.0,
                            263130836933693530167218012160000000.0 };

  double loop_factorial(unsigned int begin, unsigned int end)
  {
    double f = begin;
    for(unsigned int i = begin + 1; i <= end; ++i)
      f *= i;
    return f;
  }
}

double factorial(unsigned int n)
{
  return n > 32 ? loop_factorial(33, n)*fact[32] : fact[n];
}

double legendre_P(unsigned int ell, unsigned int m, double x)
{
  double pmm = 1.0;
  if(m > 0)
  {
    double somx2 = sqrt((1.0 - x)*(1.0 + x));
    double fact = 1.0;
    for(unsigned int i = 0 ; i < m; ++i)
    {
      pmm *= (-fact)*somx2;
      fact += 2.0;
    }
  }
  if(ell == m)
    return pmm;
  double pmmp1 = x*(2*m + 1.0)*pmm;
  if(ell == m + 1)
    return pmmp1;
  double pll = 0.0;
  for(unsigned int ll = m + 2; ll <= ell; ++ll)
  {
    pll = ((2.0*ll - 1.0)*x*pmmp1 - (ll + m - 1.0)*pmm)/static_cast<double>(ll - m);
    pmm = pmmp1;
    pmmp1 = pll;
  }
  return pll;
}

double sh_normalization(unsigned int ell, unsigned int m)
{
  return sqrt((2.0*ell + 1.0)*factorial(ell - m)/(M_4PI*factorial(ell + m)));
}

double spherical_harmonics(unsigned int ell, int m, double cos_theta, double phi)
{
  static const double sqrt2 = sqrt(2.0);
  if(m == 0)
    return sqrt((2.0*ell + 1.0)/(M_4PI))*legendre_P(ell, m, cos_theta);
  else if(m > 0)
    return sqrt2*sh_normalization(ell, m)*legendre_P(ell, m, cos_theta)*cos(m*phi);
  else
    return sqrt2*sh_normalization(ell, -m)*legendre_P(ell, -m, cos_theta)*sin(m*phi);
}

void spherical_harmonics(unsigned int no_of_bands, double cos_theta, double phi, valarray<double>& sh_result)
{
  sh_result.resize(no_of_bands*no_of_bands);
  for(int ell = 0; ell < static_cast<int>(no_of_bands); ++ell)
    for(int m = -ell; m <= ell; ++m)
    {
      unsigned int index = ell*(ell + 1) + m;
      sh_result[index] = spherical_harmonics(ell, m, cos_theta, phi);
    }
}
