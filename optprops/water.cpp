// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2010
// Copyright (c) DTU Informatics 2010

#include <complex>
#include "Medium.h"
#include "water.h"

using namespace std;

#ifndef M_PI
#define M_PI 3.141592653589793238462643
#endif

namespace
{
  const unsigned int no_of_samples = 17;
  const double first_wavelength = 375.0;
  const double wavelength_step_size = 25.0;

  // Data from Pope and Fry 1997 and Hale and Querry 1973.
  const double ior_imag_part[] = { 3.393e-10,
                                   2.110e-10,
                                   1.617e-10,
                                   3.302e-10,
                                   4.309e-10,
                                   8.117e-10,
                                   1.742e-9,
                                   2.473e-9,
                                   3.532e-9,
                                   1.062e-8,
                                   1.410e-8,
                                   1.759e-8,
                                   2.406e-8,
                                   3.476e-8,
                                   8.591e-8,
                                   1.474e-7,
                                   1.486e-7  };

  // Data from Pegau et al. 1997.
  const double TS_correction[17][2] = { {  1.0e-4,  1.2e-4 },
                                     {  1.0e-4,  1.2e-4 },
                                     {  0.5e-4,  5.5e-5 },
                                     {  0.0   , -2.0e-5 },
                                     {  0.0   , -2.0e-5 },
                                     {  1.0e-4, -2.0e-5 },
                                     {  2.0e-4, -2.5e-5 },
                                     {  1.0e-4, -3.0e-5 },
                                     {  2.0e-4, -2.0e-5 },
                                     {  1.0e-3, -1.5e-5 },
                                     {  5.0e-4, -1.0e-5 },
                                     {  0.5e-4,  0.0    },
                                     {  1.0e-4, -2.0e-5 },
                                     {  2.0e-4, -1.7e-4 },
                                     {  6.5e-3, -1.0e-5 },
                                     { 10.6e-3,  6.4e-4 },
                                     { 10.6e-3,  6.4e-4 }  };
}

Medium water(double T, double S)
{
  static const double M_4PI = 4.0*M_PI;

  Medium m;
  Color< complex<double> >& ior = m.get_ior(spectrum);
  ior.resize(no_of_samples);
  ior.wavelength = first_wavelength;
  ior.step_size = wavelength_step_size;

  for(unsigned int i = 0; i < ior.size(); ++i)
  {
    double lambda = ior.wavelength + i*ior.step_size;
    double Psi_T = TS_correction[i][0];
    double Psi_S = TS_correction[i][1];
    double ior_imag = ior_imag_part[i] + lambda*((T - 22)*Psi_T + S*Psi_S)/M_4PI;
    ior[i] = complex<double>(water_ior_real_part(lambda, T, S), ior_imag);
  }

  m.name = "water";
  return m;  
}

// Empirical formula from Quan and Fry 1995
double water_ior_real_part(double lambda, double T, double S)
{
  const double n[10] = {  1.31405,  1.799e-4, -1.05e-6,     1.6e-8, -2.02e-6, 
                         15.868   ,  0.01155, -0.00423, -4382.0   ,  1.1455e6 };
  double T_sq = T*T;
  double lambda_sq = lambda*lambda;
  return n[0] + (n[1] + n[2]*T + n[3]*T_sq)*S + n[4]*T_sq 
    + (n[5] + n[6]*S + n[7]*T)/lambda + n[8]/lambda_sq + n[9]/(lambda_sq*lambda);
}
