// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef FRESNEL_H
#define FRESNEL_H

#include <cmath>
#include <complex>

inline double fresnel_r_s(double cos_theta1, double cos_theta2, double ior1, double ior2)
{
  // Compute the perpendicularly polarized component of the Fresnel reflectance
  return 0.0;
}

inline double fresnel_r_p(double cos_theta1, double cos_theta2, double ior1, double ior2)
{
  // Compute the parallelly polarized component of the Fresnel reflectance
  return 0.0;
}

inline double fresnel_R(double cos_theta1, double cos_theta2, double ior1, double ior2)
{
  // Compute the Fresnel reflectance using fresnel_r_s(...) and fresnel_r_p(...)
  return 0.0;
}

inline double fresnel_R(double cos_theta, double ior1, double ior2)
{
  // Compute cos_theta2 and use fresnel_R(...) to find the Fresnel reflectance
  return 0.0;
}

inline double fresnel_R(double cos_theta, std::complex<double> ior1, std::complex<double> ior2)
{
  // Compute the Fresnel reflectance for complex indices of refraction.
  return 0.0;
}

inline double fresnel_R_s(double cos_theta, std::complex<double> ior1, std::complex<double> ior2)
{
  // Compute the perpendicularly polarized component of the Fresnel reflectance
  // for complex indices of refraction
  return 0.0;
}

inline double fresnel_R_p(double cos_theta, std::complex<double> ior1, std::complex<double> ior2)
{
  // Compute the parallelly polarized component of the Fresnel reflectance
  // for complex indices of refraction
  return 0.0;
}

#endif // FRESNEL_H
