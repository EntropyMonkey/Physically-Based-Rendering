/*
 * The following source code implements Lorenz-Mie theory
 * using the formulas presented in the SIGGRAPH 2007 paper
 *
 * Computing the Scattering Properties of Participating
 * Media Using Lorenz-Mie Theory
 *
 * By authors: Jeppe Revall Frisvad
 *             Niels Joergen Christensen
 *             Henrik Wann Jensen
 *
 * Code written by Jeppe Revall Frisvad, 2006.
 * Copyright (c) ACM 2007
 *
 * The ACM copyright covers publication of the code in 
 * whole or in part in any and all forms of media.
 * 
 * Permission is granted to anyone to use this code as 
 * software for any purpose, including commercial applications.
 * However, the software is provided 'as-is' without any warranty.
 */ 

#ifndef LORENZMIE_H
#define LORENZMIE_H

#include <complex>
#include <valarray>

namespace LorenzMie
{
  struct ParticleDistrib
  {
    double ext, sca, abs, g;
    double ior;
    std::complex<double> refrac_idx;
    double r_min, r_max;
    double dr;
    std::valarray<double> N;
  };

  // Function computing optical properties for a distribution
  // of particles in an absorbing host medium.
  //
  // Output:
  // p.ext - extinction coefficient
  // p.sca - scattering coefficient
  // p.abs - absorption coefficient
  // p.g   - bulk asymmetry parameter
  // p.ior - integral over corrections for the real 
  //         part of the bulk index of refraction
  //         (should be summed with contributions
  //         from other particle types and multiplied
  //         by the wavelength, see Equation 33)
  //
  // Input:
  // p.refrac_idx    - refractive index of the particles
  // p.r_min         - minimum particle radius
  // p.r_max         - maximum particle radius
  // p.dr            - step size for piecewise integration
  // p.N             - number of particles for each interval
  // wavelength      - wavelength of incident light
  // host_refrac     - refractive index of host medium
  // particle_refrac - (optional) if the refractive index
  //                   of the particles is wavelength
  //                   dependent, this argument can be
  //                   given to replace p.refrac_idx
  // 
  void optical_props(ParticleDistrib* p, 
                     double wavelength, 
                     const std::complex<double>& host_refrac,
                     const std::complex<double>* particle_refrac = 0);
  
  // Function computing optical properties for one particle 
  // embedded in an absorbing host medium.
  //
  // Output:
  // C_t - extinction cross section
  // C_s - scattering cross section
  // C_a - absorption cross section
  // g   - asymmetry parameter
  // ior - correction for the real part
  //       of the bulk index of refraction
  //
  // Input:
  // radius          - particle radius
  // wavelength      - wavelength of incident light
  // host_refrac     - refractive index of the host medium
  // particle_refrac - refractive index of the particle
  //
  void particle_props(double& C_t, double& C_s, double& C_a, 
                      double& g, double& ior,
                      double radius, double wavelength,
                      const std::complex<double>& host_refrac,
                      const std::complex<double>& particle_refrac);
}

#endif // LORENZMIE_H
