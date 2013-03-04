// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2010
// Copyright (c) DTU Informatics 2010

#include <cmath>
#include <complex>
#include "CGLA/CGLA.h"
#include "Medium.h"
#include "incandescent.h"

using namespace std;

namespace
{
  const unsigned int no_of_samples = 17;
  const double first_wavelength = 375.0;
  const double wavelength_step_size = 25.0;
}

Medium blackbody(double T)         // temperature [K]
{
  const double h = 6.62606896e-34; // Planck's constant [J*s]
  const double k = 1.3806504e-23;  // Boltzmann's constant [J/K]
  const double c = 2.99792458e8;   // speed of light [m/s]

  Medium m;
  Color< complex<double> >& ior = m.get_ior(mono);
  ior.resize(1, complex<double>(1.0, 0.0));
  Color<double>& emission = m.get_emission(spectrum);
  emission.resize(no_of_samples);
  emission.wavelength = first_wavelength;
  emission.step_size = wavelength_step_size;

  // Using Planck's blackbody emission spectrum
  for(unsigned int i = 0; i < emission.size(); ++i)
  {
    double lambda = (emission.wavelength + i*emission.step_size)*1.0e-9; // wavelength [m]

    emission[i] = 2.0*M_PI*c*c*h/(CGLA::int_pow(lambda, 5)*(exp(h*c/(lambda*k*T)) - 1.0))*25.0e-9;
  }

  m.name = "blackbody";
  m.emissive = true;
  return m;
}

Medium incandescent(double power,         // power [W]
                    double bulb_area,     // bulb surface area [m^2]
                    double filament_area) // filament surface area [m^2]
{
  const double sigma = 5.6704e-8;         // Stefan-Boltzmann constant [W/(m^2*K^4)]

  // Using the Stefan-Boltzmann law of radiation: M = sigma*T^4
  double T = pow(power/(filament_area*sigma), 0.25);

  Medium m = blackbody(T);
  Color<double>& emission = m.get_emission(spectrum);

  for(unsigned int i = 0; i < emission.size(); ++i)
    emission[i] *= filament_area/bulb_area;

  m.name = "incandescent";
  return m;
}
