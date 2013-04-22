// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef FRESNEL_H
#define FRESNEL_H

#include <cmath>
#include <complex>

inline double fresnel_r_p(double cos_theta1, double cos_theta2, double ior1, double ior2)
{
  // Compute the parallelly polarized component of the Fresnel reflectance
  return (ior2 * cos_theta1 - ior1 * cos_theta2) / (ior2 * cos_theta1 + ior1 * cos_theta2);
}

inline double fresnel_r_s(double cos_theta1, double cos_theta2, double ior1, double ior2)
{
  // Compute the perpendicularly polarized component of the Fresnel reflectance
  return (ior1 * cos_theta1 - ior2 * cos_theta2) / (ior1 * cos_theta1 + ior2 * cos_theta2);
}

inline double fresnel_R(double cos_theta1, double cos_theta2, double ior1, double ior2)
{
  // Compute the Fresnel reflectance using fresnel_r_s(...) and fresnel_r_p(...)
  double r_s = fresnel_r_s(cos_theta1, cos_theta2, ior1, ior2);
  double r_p = fresnel_r_p(cos_theta1, cos_theta2, ior1, ior2);
  return (r_s * r_s + r_p * r_p) * 0.5f;
}

inline double fresnel_R(double cos_theta, double ior1, double ior2)
{
  // Compute cos_theta2 and use fresnel_R(...) to find the Fresnel reflectance

  // use pythagorean identity and snell's law
  double sin_theta1 = sqrt(1 - cos_theta * cos_theta);
  double sin_theta2 = ior1 / ior2 * sin_theta1;
  double cos_theta2 = sqrt(1.0 - sin_theta2 * sin_theta2);

  return fresnel_R(cos_theta, cos_theta2, ior1, ior2);
}

inline std::complex<double> fresnel_R_s(double cos_theta, std::complex<double> cos_theta2, std::complex<double> ior1, std::complex<double> ior2)
{
  // Compute the perpendicularly polarized component of the Fresnel reflectance
  // for complex indices of refraction
  return (ior1 * cos_theta - ior2 * cos_theta2) / (ior1 * cos_theta + ior2 * cos_theta2);
}

inline std::complex<double> fresnel_R_p(double cos_theta, std::complex<double> cos_theta2, std::complex<double> ior1, std::complex<double> ior2)
{
  // Compute the parallelly polarized component of the Fresnel reflectance
  // for complex indices of refraction
  return (ior2 * cos_theta - ior1 * cos_theta2) / (ior2 * cos_theta + ior1 * cos_theta2);
}

inline double fresnel_R(double cos_theta, std::complex<double> ior1, std::complex<double> ior2)
{
  // Compute the Fresnel reflectance for complex indices of refraction.

  // use pythagorean identity and snell's law
  double sin_theta1 = sqrt(1 - cos_theta * cos_theta);
  std::complex<double> sin_theta2 = ior1 / ior2 * sin_theta1;
  std::complex<double> cos_theta2 = sqrt(1.0 - sin_theta2 * sin_theta2);

  std::complex<double> r_p = fresnel_R_p(cos_theta, cos_theta2, ior1, ior2);
  std::complex<double> r_s = fresnel_R_s(cos_theta, cos_theta2, ior1, ior2);
  
  return (abs(r_s * r_s) + abs(r_p * r_p)) * 0.5;
}


#endif // FRESNEL_H
