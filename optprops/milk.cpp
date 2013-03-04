// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#include <iostream>
#include <complex>
#include <cmath>
#include <valarray>
#include "LorenzMie.h"
#include "Medium.h"
#include "milk.h"

#ifndef M_PI
  const double M_PI = 3.14159265358979323846;
#endif

using namespace std;
using namespace LorenzMie;

namespace
{
  // Data from Hale and Querry 1973
  complex<double> refrac_water[] = { complex<double>(1.341, 3.393e-10),
                                     complex<double>(1.339, 2.110e-10),
                                     complex<double>(1.338, 1.617e-10),
                                     complex<double>(1.337, 3.302e-10),
                                     complex<double>(1.336, 4.309e-10),
                                     complex<double>(1.335, 8.117e-10),
                                     complex<double>(1.334, 1.742e-9),
                                     complex<double>(1.333, 2.473e-9),
                                     complex<double>(1.333, 3.532e-9),
                                     complex<double>(1.332, 1.062e-8),
                                     complex<double>(1.332, 1.410e-8),
                                     complex<double>(1.331, 1.759e-8),
                                     complex<double>(1.331, 2.406e-8),
                                     complex<double>(1.331, 3.476e-8),
                                     complex<double>(1.330, 8.591e-8),
                                     complex<double>(1.330, 1.474e-7),
                                     complex<double>(1.330, 1.486e-7)  };

  // Absorbance from Du et al. 1998 (PhotochemCAD 1)
  double abs_riboflavin[] = { 0.07117, 0.05934, 0.07215, 0.08300,
                              0.06379, 0.01962, 0.00950, 0.00957,
                              0.01198, 0.01047, 0.01046, 0.01061,
                              0.00671, 0.01034, 0.00589, 0.00886, 0.01015 };

  // Data from Michalski et al. 2001
  complex<double> refrac_fat[] = { complex<double>(1.480, 4.0e-6),
                                   complex<double>(1.476, 6.4e-6),
                                   complex<double>(1.473, 8.6e-6),
                                   complex<double>(1.470, 1.14e-5),
                                   complex<double>(1.468, 1.10e-5),
                                   complex<double>(1.466, 1.00e-5),
                                   complex<double>(1.464, 4.7e-6),
                                   complex<double>(1.463, 4.6e-6),
                                   complex<double>(1.462, 4.7e-6),
                                   complex<double>(1.461, 4.9e-6),
                                   complex<double>(1.460, 5.0e-6),
                                   complex<double>(1.459, 5.0e-6),
                                   complex<double>(1.458, 5.1e-6),
                                   complex<double>(1.457, 5.2e-6),
                                   complex<double>(1.457, 5.2e-6),
                                   complex<double>(1.456, 5.2e-6),
                                   complex<double>(1.456, 5.2e-6)  };

  template<class T> void init_spectrum(Color<T>& c, unsigned int no_of_samples)
  {
    c.resize(no_of_samples);
    c.wavelength = 375.0;
    c.step_size = 25.0;
  }

  double meter2micro(double r)
  {
    return r*1.0e6;
  }

  double casein_transform(double r)
  {
    return r/(150.0e-9 - r);
  }

  void set_number_densities(LogNormalParticleDistrib& distrib, double volume, double (*transform)(double))
  {
    static const double M_SQRT_2PI = sqrt(2.0*M_PI);
    int counter;
    distrib.N.resize(static_cast<unsigned int>((distrib.r_max - distrib.r_min)/distrib.dr) + 1);

    double alpha, beta, beta_sqr;
    if(volume <= 0.0)
    {
      alpha = 0.0;
      beta = 1.0;
    }
    else
    {
      double r_vs_tr = transform(distrib.r_vs);
      beta_sqr = log(distrib.c_s*distrib.c_s/(r_vs_tr*r_vs_tr) + 1.0);
      alpha = log(r_vs_tr) - 0.5*beta_sqr;
      beta = sqrt(beta_sqr);
    }
    double distrib_V = 0.0;
    counter = 0;
    for(double r = distrib.r_min + distrib.dr*0.5; r < distrib.r_max; r += distrib.dr)
    {
      double r_tr = transform(r);
      double tmp = (log(r_tr) - alpha)/beta;
      distrib.N[counter] = 1.0/(r_tr*beta*M_SQRT_2PI)*exp(-0.5*tmp*tmp);
      distrib_V += distrib.N[counter];
      ++counter;
    }
    distrib.N *= volume/distrib_V;

    counter = 0;
    for(double r = distrib.r_min + distrib.dr*0.5; r < distrib.r_max; r += distrib.dr)
      distrib.N[counter++] *= 3.0/(4.0*M_PI*r*r*r)*1.0/distrib.dr;
  }
}

Medium unhomogenized_milk(double fat_weight,     // milk fat weight-%
                          double protein_weight, // protein weight-%
                          double casein_r_vs,    // volume-to-surface mean radius of casein micelles
                          double casein_c_s)     // radius standard deviation of casein micelles
{
  return homogenized_milk(fat_weight, protein_weight, 0.0, casein_r_vs, casein_c_s);
}

Medium homogenized_milk(double fat_weight,     // milk fat weight-%
                        double protein_weight, // protein weight-%
                        double pressure,       // Homogenization pressure
                        double casein_r_vs,    // volume-to-surface mean radius of casein micelles
                        double casein_c_s)     // radius standard deviation of casein micelles
{
  double fat_r_vs;
  double fat_c_s;

  // Functional expresions finding fat globule size distribution
  const double log_50 = log10(50.0);
  const double v_sqr = fat_weight*fat_weight;
  const double r_P50 = 7.481403922e-4*v_sqr + 3.203991829e-2*fat_weight + 0.1162155667;
  const double r_P0 = fat_weight < 2.0 
                      ? -0.2528033794*v_sqr + 1.418606759*fat_weight 
                      : 1.456189142*pow(fat_weight, 0.36);
  const double beta = fat_weight > 0.0 ? log10(r_P0) : 0.0;
  const double alpha = beta/log_50 - log10(r_P50)/log_50;

  const double cv = 0.6;
  fat_r_vs = (pressure > 0.0 ? pow(10.0, -alpha*log10(pressure) + beta) : r_P0)/(cv*cv + 1.0);
  fat_c_s = fat_r_vs*cv;
  fat_r_vs *= 1.0e-6;

  //cerr << "Volume-to-area mean radius of fat globules: " << fat_r_vs << endl;
  //cerr << "Radius standard deviation of fat globules: " << fat_c_s << endl;

  return milk(fat_weight, protein_weight, fat_r_vs, fat_c_s, casein_r_vs, casein_c_s);
}

Medium milk(double fat_weight,      // milk fat weight-%
            double protein_weight,  // protein weight-%
            double fat_r_vs,        // volume-to-surface mean radius of fat globules
            double fat_c_s,         // radius standard deviation of fat globules
            double casein_r_vs,     // volume-to-surface mean radius of casein micelles
            double casein_c_s)      // radius standard deviation of casein micelles
{
  LogNormalParticleDistrib fat, casein;
  fat.r_vs = fat_r_vs;
  fat.c_s = fat_c_s;
  fat.r_min = 0.005e-6;
  fat.r_max = 10.0e-6;
  fat.dr = 0.05e-6;
  casein.r_vs = casein_r_vs;
  casein.c_s = casein_c_s;
  casein.r_min = 0.0e-9;
  casein.r_max = 150.0e-9;
  casein.dr = 1.0e-9;
  return milk(fat, casein, fat_weight, protein_weight);
}

Medium milk(LogNormalParticleDistrib& fat,    // Distribution of fat globules
            LogNormalParticleDistrib& casein, // Distribution of casein micelles
            double fat_weight,                // milk fat weight-%
            double protein_weight)            // protein weight-%
{
  static const double fat_density = 0.915;
  static const double casein_density = 1.11;
  static const double milk_density = 1.030;

  // More input data:
  fat.refrac_idx = complex<double>(1.462, 0.0);
  casein.refrac_idx =  complex<double>(1.503, 0.0);

  double fat_volume = (fat_weight/fat_density)/(100.0/milk_density);
  double casein_volume = 0.76*(protein_weight/casein_density)/(100.0/milk_density);
  //cerr << "Fat volume fraction: " << fat_volume << endl;
  //cerr << "Casein volume fraction: " << casein_volume << endl;

  set_number_densities(fat, fat_volume, meter2micro);
  set_number_densities(casein, casein_volume, casein_transform);

  return milk(fat, casein);
}

Medium milk(LogNormalParticleDistrib& fat,    // Distribution of fat globules
            LogNormalParticleDistrib& casein) // Distribution of casein micelles
{
  static const int NO_SAMPLES = 17;
  static const double milk_density = 1.030;
  Medium m;
  complex<double> refrac_host[NO_SAMPLES];
  Color< complex<double> >& ior = m.get_ior(spectrum);
  Color<double>& extinct = m.get_extinction(spectrum);
  Color<double>& scatter = m.get_scattering(spectrum);
  Color<double>& absorp = m.get_absorption(spectrum);
  Color<double>& asymmetry = m.get_asymmetry(spectrum);
  double abs_ribo[17];
  int counter;

  // Initialize medium
  init_spectrum(ior, NO_SAMPLES);
  init_spectrum(extinct, NO_SAMPLES);
  init_spectrum(scatter, NO_SAMPLES);
  init_spectrum(absorp, NO_SAMPLES);
  init_spectrum(asymmetry, NO_SAMPLES);

  // Conversion from molar absorbance to molar absorption coefficients.
  // Scale based on molar absorption coefficient: 33000 M^-1 cm^-1 for 266.5 nm (Koziol 1966),
  // where absorbance was measured to be 0.25654.
  double scale = 3.3e6/0.25654;
  for(int i = 0; i < NO_SAMPLES; ++i)
    abs_ribo[i] = scale*abs_riboflavin[i];

  // Weight fraction of riboflavin in milk (Fox and McSweeney 1998).
  // Natural content: 0.17 mg pr. 100 g.
  // Molar mass of riboflavin (C_17 H_20 N_4 O_6): 376.3682 g/mol
  const double w_riboflavin = log(10.0)*(0.17/376.3682)/(100.0/milk_density);

  // Correction of host medium refractive index
  counter = 0;
  for(double wavelength = 375.0e-9; wavelength < 780.0e-9; wavelength += 25.0e-9)
  {
    double refrac_ribo_imag = w_riboflavin*abs_ribo[counter]*wavelength/(4.0*M_PI);
    refrac_host[counter] = complex<double>(refrac_water[counter].real(), refrac_water[counter].imag() + refrac_ribo_imag);
    //cerr << refrac_water[counter].imag()
    //     << " " << refrac_ribo_imag
    //     << " " << refrac_host[counter].imag() << endl;
    ++counter;
  }

  for(double wavelength = 375.0e-9; wavelength < 780.0e-9; wavelength += 25.0e-9)
  {
    unsigned int wave_idx = static_cast<unsigned int>((wavelength - 375.0e-9)/24.9e-9);
    //cerr << "Wavelength: " << wavelength << " (idx: " << wave_idx << ")" << endl;
    //cerr << "Refractive Index of host medium: " << refrac_host[wave_idx] << endl;

    optical_props(&fat, wavelength, refrac_host[wave_idx], &refrac_fat[wave_idx]);
    //cerr << "Fat properties: " << fat.ext << " " << fat.sca << " " << fat.abs << " " << fat.g << endl;

    optical_props(&casein, wavelength, refrac_host[wave_idx], &casein.refrac_idx);
    //cerr << "Casein properties: " << casein.ext << " " << casein.sca << " " << casein.abs << " " << casein.g << endl;

    double water_absorp = 4.0*M_PI*refrac_host[wave_idx].imag()/wavelength;
    //cerr << "Host extinction: " << water_absorp << endl;
    if(casein.sca - casein.ext > water_absorp)
    {
      cerr << "Lorenz-Mie coefficients did not converge or integration is too coarse." << endl;
      exit(0);
    }

    extinct[wave_idx] = fat.ext + casein.ext + water_absorp;
    scatter[wave_idx] = fat.sca + casein.sca;
    absorp[wave_idx] = extinct[wave_idx] - scatter[wave_idx];
    ior[wave_idx] = complex<double>(refrac_host[wave_idx].real(), absorp[wave_idx]*wavelength/(4.0*M_PI));
    if(scatter[wave_idx] != 0.0)
      asymmetry[wave_idx] = (fat.sca*fat.g + casein.sca*casein.g)/scatter[wave_idx];

    //cout << "Extinction coefficient (" << wavelength*1e9 << "nm): " << extinct[wave_idx] << endl
    // << "Scattering coefficient (" << wavelength*1e9 << "nm): " << scatter[wave_idx] << endl
    // << "Absorption coefficient (" << wavelength*1e9 << "nm): " << absorp[wave_idx] << endl
    // //<< "Fat asymmetry parameter (" << wavelength*1e9 << "nm):    " << fat.g << endl
    // //<< "Casein asymmetry parameter (" << wavelength*1e9 << "nm):    " << casein.g << endl
    // << "Ensemble asymmetry parameter (" << wavelength*1e9 << "nm):    " << asymmetry[wave_idx] << endl;

    //if(wavelength < 774.0e-9)
    //{
    //  cout << endl << "<press enter for next set of values>" << endl;
    //  cin.get();
    //}
  }
  m.name = "milk";
  m.turbid = true;
  return m;
}