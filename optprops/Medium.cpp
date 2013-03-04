// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2010
// Copyright (c) DTU Informatics 2010

#include <iostream>
#include <valarray>
#include <numeric>
#include "spectrum2xyz.h"
#include "spectrum2rgb.h"
#include "Medium.h"

using namespace std;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace
{
  template<class T> inline T mean(valarray<T>& v)
  {
    return accumulate(&v[0], &v[v.size()], T(0.0))/static_cast<double>(v.size());
  }

  double ior_to_abs(const complex<double>& ior, double wavelength)
  {
    return 4.0*M_PI*ior.imag()/wavelength;
  }

  Color<double> ior_spectrum_to_abs(Color< complex<double> >& ior)
  {
    Color<double> abs(ior.size(), ior.unit);
    abs.wavelength = ior.wavelength;
    abs.step_size = ior.step_size;
    double lambda = unit_to_iso(ior.wavelength, ior.unit);
    for(unsigned int i = 0; i < ior.size(); ++i)
    {
      abs[i] = ior_to_abs(ior[i], lambda);
      lambda += unit_to_iso(ior.step_size, ior.unit);
    }
    return abs;
  }

  template<class T> Color<T> spectrum_to_xyz(const Color<T>& spec)
  {
    T v_xyz[3] = { T(0.0), T(0.0), T(0.0) };
    double xyz_sums[3] = { 0.0, 0.0, 0.0 };
    double lambda = 
      spec.unit == nanometers 
      ? spec.wavelength 
      : unit_to_iso(spec.wavelength, spec.unit)*1.0e9;
    double step_size = unit_to_iso(spec.step_size, spec.unit)*1.0e9;
    for(unsigned int i = 0; i < spec.size(); ++i)
    {
      unsigned int idx = get_nearest_xyz_index(lambda);
      if(idx == CIE_xyz_samples - 1)
        break;

      double c = (lambda - CIE_xyz[idx][0])/(CIE_xyz[idx + 1][0] - CIE_xyz[idx][0]);
      for(unsigned int j = 1; j < 4; ++j)
      {
        double cie = (1.0 - c)*CIE_xyz[idx][j] + c*CIE_xyz[idx + 1][j];
        v_xyz[j - 1] += cie*spec[i];
        xyz_sums[j - 1] += cie;
      }
      lambda += step_size;
    }
    for(int i = 0; i < 3; ++i)
      v_xyz[i] /= xyz_sums[i];
    return Color<T>(v_xyz, 3);
  }

  template<class T> Color<T> spectrum_to_rgb(const Color<T>& spec)
  {
    T v_rgb[3] = { T(0.0), T(0.0), T(0.0) };
    double rgb_sums[3] = { 0.0, 0.0, 0.0 };
    double lambda = 
      spec.unit == nanometers 
      ? spec.wavelength 
      : unit_to_iso(spec.wavelength, spec.unit)*1.0e9;
    double step_size = unit_to_iso(spec.step_size, spec.unit)*1.0e9;
    for(unsigned int i = 0; i < spec.size(); ++i)
    {
      unsigned int idx = get_nearest_rgb_index(lambda);
      if(idx == spectrum_rgb_samples - 1)
        break;

      double c = (lambda - spectrum_rgb[idx][0])/(spectrum_rgb[idx + 1][0] - spectrum_rgb[idx][0]);
      for(unsigned int j = 1; j < 4; ++j)
      {
        double cie = (1.0 - c)*spectrum_rgb[idx][j] + c*spectrum_rgb[idx + 1][j];
        v_rgb[j - 1] += cie*spec[i];
        rgb_sums[j - 1] += cie;
      }
      lambda += step_size;
    }
    for(int i = 0; i < 3; ++i)
      v_rgb[i] /= rgb_sums[i];
    return Color<T>(v_rgb, 3);
  }

  // This should be implemented using Glassner's "Principles"
  template<class T> Color<T> xyz_to_spectrum(const Color<T>& v_xyz, unsigned int samples)
  { 
    return Color<T>();
  }
    
  // Using the standard conversion matrix recommended for unknown white point
  template<class T> Color<T> xyz_to_rgb(const Color<T>& v_xyz)
  {
    T v_rgb[3];
    v_rgb[0] =  3.2405*v_xyz[0] - 1.5371*v_xyz[1] - 0.4985*v_xyz[2];
    v_rgb[1] = -0.9693*v_xyz[0] + 1.8760*v_xyz[1] + 0.0416*v_xyz[2];
    v_rgb[2] =  0.0556*v_xyz[0] - 0.2040*v_xyz[1] + 1.0572*v_xyz[2];
    return Color<T>(v_rgb, 3); 
  }

  // Using the standard conversion matrix recommended for unknown white point
  template<class T> Color<T> rgb_to_xyz(const Color<T>& v_rgb)
  {
    T v_xyz[3];
    v_xyz[0] = 0.4124*v_rgb[0] + 0.3576*v_rgb[1] + 0.1805*v_rgb[2];
    v_xyz[1] = 0.2126*v_rgb[0] + 0.7152*v_rgb[1] + 0.0722*v_rgb[2];
    v_xyz[2] = 0.0193*v_rgb[0] + 0.1192*v_rgb[1] + 0.9505*v_rgb[2];
    return Color<T>(v_xyz, 3); 
  }

  template<class T> void multi_to_mono(Color<T> param[4])
  {
    for(unsigned int i = 0; i < 4; ++i)
    {
      if(param[i].size() > 0)
      {
        param[mono].resize(1);
        param[mono][0] = mean(param[i]);
        break;
      }
    }
  }

  template<class T> void multi_to_rgb(Color<T> param[4])
  {
    if(param[spectrum].size() > 0)
    {
      param[rgb].resize(3);
      param[rgb] = spectrum_to_rgb(param[spectrum]);
    }
    else if(param[xyz].size() == 3)
      param[rgb] = xyz_to_rgb(param[xyz]);
    else if(param[mono].size() == 1)
    {
      param[rgb].resize(3);
      for(unsigned int i = 0; i < param[rgb].size(); ++i)
        param[rgb][i] = param[mono][0];
    }
  }

  template<class T> void multi_to_xyz(Color<T> param[4])
  {
    if(param[spectrum].size() > 0)
    {
      param[xyz].resize(3);
      param[xyz] = spectrum_to_xyz(param[spectrum]);
    }
    else if(param[rgb].size() == 3)
      param[xyz] = rgb_to_xyz(param[rgb]);
    else if(param[mono].size() == 1)
    {
      param[rgb].resize(3);
      for(unsigned int i = 0; i < param[rgb].size(); ++i)
        param[rgb][i] = param[mono][0];
      param[xyz] = rgb_to_xyz(param[rgb]);
    }
  }

  template<class T> void multi_to_spectrum(Color<T> param[4], unsigned int samples)
  {
    if(param[xyz].size() == 3)
      param[spectrum] = xyz_to_spectrum(param[xyz], samples);
    else if(param[rgb].size() == 3)
    {
      param[xyz] = rgb_to_xyz(param[rgb]);
      param[spectrum] = xyz_to_spectrum(param[xyz], samples);
    }
    else if(param[mono].size() == 1)
    {
      param[spectrum].resize(samples);
      for(unsigned int i = 0; i < param[spectrum].size(); ++i)
        param[spectrum][i] = param[mono][0];
    }
  }

  template<class T>
  void single_to_rgb(double wavelength, const Color<T> &spec_col, Color<T> &rgb_col)
  {
    if(spec_col.size() > 0)
    {
      if(rgb_col.size() != 3)
        rgb_col.resize(3);

      T v = spec_col.get_linear(wavelength);
      for(unsigned int i = 0; i < rgb_col.size(); ++i)
        rgb_col[i] = v;
    }
  }
}

double unit_to_iso(double value, SpectralUnit u)
{
  switch(u)
  {
  case kilometers: return value*1.0e3;
  case millimeters: return value*1.0e-3;
  case micrometers: return value*1.0e-6;
  case nanometers: return value*1.0e-9;
  case angstrom: return value*1.0e-10;
  case electron_volt: return 1240.0e-9/value;
  }
  return 1.0;
}

double unit_to_iso(double value, ScatteringUnit u)
{
  switch(u)
  {
  case per_cm: return value*1.0e2;
  case per_mm: return value*1.0e3;
  }
  return 1.0;
}

void Medium::fill_data(ColorType type)
{
  switch(type)
  {
  case mono:
    fill_mono_data();
    break;
  case rgb:
    fill_rgb_data();
    break;
  case xyz:
    fill_xyz_data();
    break;
  case spectrum:
    fill_spectral_data();
    break;
  }
}

void Medium::fill_mono_data()
{
  if(ior[mono].size() != 1)
  {
    multi_to_mono(ior);
    if(ior[mono].size() != 1)
    {
      cerr << "Warning: Index of refraction not available for " << name << "." << endl
           << "  Using default: IOR = 1" << endl;
      ior[mono].resize(1, complex<double>(1.0, 0.0));
    }
  }

  if(absorption[mono].size() != 1)
  {
    multi_to_mono(absorption);
    if(absorption[mono].size() != 1)
      absorption[mono].resize(1, ior_to_abs(ior[mono][0], 575.0e-9));
  }

  if(emissive && emission[mono].size() != 1)
  {
    multi_to_mono(emission);
    if(emission[mono].size() != 1)
    {
      cerr << "Warning: Emission not available for " << name << "." << endl
           << "  Using default: L_e = 0" << endl;
      emission[mono].resize(1, 0.0); 
    }
  }

  if(turbid)
  {
    if(scattering[mono].size() != 1)
    {
      multi_to_mono(scattering);
      if(scattering[mono].size() != 1)
      {
        cerr << "Warning: Scattering coefficient not available for " << name << "." << endl
             << "  Using default: sigma_s = 0" << endl;
        scattering[mono].resize(1, 0.0);
      }
    }
    if(asymmetry[mono].size() != 1)
    {
      multi_to_mono(asymmetry);
      if(asymmetry[mono].size() != 1)
      {
        cerr << "Warning: Asymmetry parameter not available for " << name << "." << endl
             << "  Using default: g = 0" << endl;
        asymmetry[mono].resize(1, 0.0);
      }
    }
  }
  else
  {
    if(scattering[mono].size() != 1)
      scattering[mono].resize(1, 0.0);
    if(asymmetry[mono].size() != 1)
      asymmetry[mono].resize(1, 0.0);
  }
  if(extinction[mono].size() != 1)
    extinction[mono].resize(1, scattering[mono][0] + absorption[mono][0]);
  if(albedo[mono].size() != 1)
  {
    if(extinction[mono][0] == 0.0)
      albedo[mono].resize(1, 0.0);
    else
      albedo[mono].resize(1, scattering[mono][0]/extinction[mono][0]);      
  }
  if(reduced_sca[mono].size() != 1)
    reduced_sca[mono].resize(1, scattering[mono][0]*(1.0 - asymmetry[mono][0]));
  if(reduced_ext[mono].size() != 1)
    reduced_ext[mono].resize(1, reduced_sca[mono][0] + absorption[mono][0]);
  if(reduced_alb[mono].size() != 1)
  {
    if(extinction[mono][0] == 0.0)
      reduced_alb[mono].resize(1, 0.0);
    else
      reduced_alb[mono].resize(1, reduced_sca[mono][0]/reduced_ext[mono][0]);
  }
}

void Medium::fill_rgb_data()
{
  if(ior[rgb].size() != 3)
  {
    multi_to_rgb(ior);
    if(ior[rgb].size() != 3)
    {
      cerr << "Warning: Index of refraction not available for " << name << "." << endl
           << "  Using default: IOR = 1" << endl;
      ior[rgb].resize(3, complex<double>(1.0, 0.0));
    }
    for(unsigned int i = 0; i < 3; ++i)
      if(ior[rgb][i].imag() < 0.0)
        ior[rgb][i] = complex<double>(ior[rgb][i].real(), 0.0);
  }

  if(absorption[rgb].size() != 3)
  {
    multi_to_rgb(absorption);
    if(absorption[rgb].size() != 3)
    {
      if(ior[spectrum].size() > 0)        
      {
        absorption[spectrum].resize(ior[spectrum].size());
        absorption[spectrum] = ior_spectrum_to_abs(ior[spectrum]);
        multi_to_rgb(absorption);
      }
      else
      {
        absorption[rgb].resize(3);
        absorption[rgb][0] = ior_to_abs(ior[rgb][0], 700.0e-9);
        absorption[rgb][1] = ior_to_abs(ior[rgb][1], 550.0e-9);
        absorption[rgb][2] = ior_to_abs(ior[rgb][2], 425.0e-9);
      }
    }
  }

  if(emissive && emission[rgb].size() != 3)
  {
    multi_to_rgb(emission);
    if(emission[rgb].size() != 3)
    {
      cerr << "Warning: Emission not available for " << name << "." << endl
           << "  Using default: L_e = 0" << endl;
      emission[rgb].resize(3, 0.0);
    }
    for(unsigned int i = 0; i < 3; ++i)
      if(emission[rgb][i] < 0.0)
        emission[rgb][i] = 0.0;
  }

  if(turbid)
  {
    if(scattering[rgb].size() != 3)
    {
      multi_to_rgb(scattering);
      if(scattering[rgb].size() != 3)
      {
        cerr << "Warning: Scattering coefficient not available for " << name << "." << endl
             << "  Using default: sigma_s = 0" << endl;
        scattering[rgb].resize(3, 0.0);
      }
      for(unsigned int i = 0; i < 3; ++i)
        if(scattering[rgb][i] < 0.0)
          scattering[rgb][i] = 0.0;
    }
    if(asymmetry[rgb].size() != 3)
    {
      multi_to_rgb(asymmetry);
      if(asymmetry[rgb].size() != 3)
      {
        cerr << "Warning: Asymmetry parameter not available for " << name << "." << endl
             << "  Using default: g = 0" << endl;
        asymmetry[rgb].resize(3, 0.0);
      }
      for(unsigned int i = 0; i < asymmetry[rgb].size(); ++i)
        asymmetry[rgb][i] = std::min(asymmetry[rgb][i], 1.0);
    }
  }
  else
  {
    if(scattering[rgb].size() != 3)
      scattering[rgb].resize(3, 0.0);
    if(asymmetry[rgb].size() != 3)
      asymmetry[rgb].resize(3, 0.0);
  }
  if(extinction[rgb].size() != 3)
  {
    extinction[rgb].resize(3);
    for(unsigned int i = 0; i < extinction[rgb].size(); ++i)
      extinction[rgb][i] = scattering[rgb][i] + absorption[rgb][i];
  }
  if(albedo[rgb].size() != 3)
  {
    albedo[rgb].resize(3);
    for(unsigned int i = 0; i < albedo[rgb].size(); ++i)
    {
      if(extinction[rgb][i] == 0.0)
        albedo[rgb][i] = 0.0;
      else
        albedo[rgb][i] = scattering[rgb][i]/extinction[rgb][i];
    }
  }
  if(reduced_sca[rgb].size() != 3)
  {
    reduced_sca[rgb].resize(3);
    for(unsigned int i = 0; i < reduced_sca[rgb].size(); ++i)
      reduced_sca[rgb][i] = scattering[rgb][i]*(1.0 - asymmetry[rgb][i]);
  }
  if(reduced_ext[rgb].size() != 3)
  {
    reduced_ext[rgb].resize(3);
    for(unsigned int i = 0; i < reduced_ext[rgb].size(); ++i)
      reduced_ext[rgb][i] = reduced_sca[rgb][i] + absorption[rgb][i];
  }
  if(reduced_alb[rgb].size() != 3)
  {
    reduced_alb[rgb].resize(3);
    for(unsigned int i = 0; i < reduced_alb[rgb].size(); ++i)
    {
      if(extinction[rgb][i] == 0.0)
        reduced_alb[rgb][i] = 0.0;
      else
        reduced_alb[rgb][i] = reduced_sca[rgb][i]/reduced_ext[rgb][i];
    }
  }
}

void Medium::fill_xyz_data()
{
  if(ior[xyz].size() != 3)
  {
    multi_to_xyz(ior);
    if(ior[xyz].size() != 3)
    {
      cerr << "Warning: Index of refraction not available for " << name << "." << endl
           << "  Using default: IOR = 1" << endl;
      ior[xyz].resize(3, complex<double>(1.0, 0.0));
    }
  }

  if(absorption[xyz].size() != 3)
  {
    multi_to_xyz(absorption);
    if(absorption[xyz].size() != 3)
    {
      if(ior[spectrum].size() > 0)        
      {
        absorption[spectrum].resize(ior[spectrum].size());
        absorption[spectrum] = ior_spectrum_to_abs(ior[spectrum]);
        multi_to_xyz(absorption);
      }
      else if(absorption[rgb].size() == 3)
        absorption[xyz] = rgb_to_xyz(absorption[rgb]);
      else 
      {
        if(ior[rgb].size() != 3)
          ior[rgb] = xyz_to_rgb(ior[xyz]);
        absorption[rgb].resize(3);
        absorption[rgb][0] = ior_to_abs(ior[rgb][0], 700.0e-9);
        absorption[rgb][1] = ior_to_abs(ior[rgb][1], 550.0e-9);
        absorption[rgb][2] = ior_to_abs(ior[rgb][2], 425.0e-9);
        absorption[xyz] = rgb_to_xyz(absorption[rgb]);
      }
    }
  }

  if(emissive && emission[xyz].size() != 3)
  {
    multi_to_xyz(emission);
    if(emission[xyz].size() != 3)
    {
      cerr << "Warning: Emission not available for " << name << "." << endl
           << "  Using default: L_e = 0" << endl;
      emission[xyz].resize(3, 0.0);
    }
  }

  if(turbid)
  {
    if(scattering[xyz].size() != 3)
    {
      multi_to_xyz(scattering);
      if(scattering[xyz].size() != 3)
      {
        cerr << "Warning: Scattering coefficient not available for " << name << "." << endl
             << "  Using default: sigma_s = 0" << endl;
        scattering[xyz].resize(3, 0.0);
      }
    }
    if(asymmetry[xyz].size() != 3)
    {
      multi_to_xyz(asymmetry);
      if(asymmetry[xyz].size() != 3)
      {
        cerr << "Warning: Asymmetry parameter not available for " << name << "." << endl
             << "  Using default: g = 0" << endl;
        asymmetry[xyz].resize(3, 0.0);
      }
    }
  }
  else
  {
    if(scattering[xyz].size() != 3)
      scattering[xyz].resize(3, 0.0);
    if(asymmetry[xyz].size() != 3)
      asymmetry[xyz].resize(3, 0.0);
  }
  if(extinction[xyz].size() != 3)
  {
    extinction[xyz].resize(3);
    for(unsigned int i = 0; i < extinction[xyz].size(); ++i)
      extinction[xyz][i] = scattering[xyz][i] + absorption[xyz][i];
  }
  if(albedo[xyz].size() != 3)
  {
    albedo[xyz].resize(3);
    for(unsigned int i = 0; i < albedo[xyz].size(); ++i)
    {
      if(extinction[xyz][i] == 0.0)
        albedo[xyz][i] = 0.0;
      else
        albedo[xyz][i] = scattering[xyz][i]/extinction[xyz][i];
    }
  }
  if(reduced_sca[xyz].size() != 3)
  {
    reduced_sca[xyz].resize(3);
    for(unsigned int i = 0; i < reduced_sca[xyz].size(); ++i)
      reduced_sca[xyz][i] = scattering[xyz][i]*(1.0 - asymmetry[xyz][i]);
  }
  if(reduced_ext[xyz].size() != 3)
  {
    reduced_ext[xyz].resize(3);
    for(unsigned int i = 0; i < reduced_ext[xyz].size(); ++i)
      reduced_ext[xyz][i] = reduced_sca[xyz][i] + absorption[xyz][i];
  }
  if(reduced_alb[xyz].size() != 3)
  {
    reduced_alb[xyz].resize(3);
    for(unsigned int i = 0; i < reduced_alb[xyz].size(); ++i)
    {
      if(extinction[xyz][i] == 0.0)
        reduced_alb[xyz][i] = 0.0;
      else
        reduced_alb[xyz][i] = reduced_sca[xyz][i]/reduced_ext[xyz][i];
    }
  }
}

void Medium::fill_spectral_data()
{
  if(turbid && ior[spectrum].size() != scattering[spectrum].size())
  {
    if(ior[spectrum].size() < scattering[spectrum].size())
    {
      if(ior[spectrum].size() == 0)
      {
        multi_to_spectrum(ior, scattering[spectrum].size());
        if(ior[spectrum].size() == 0)
        {
          cerr << "Warning: Index of refraction not available for " << name << "." << endl
               << "  Using default: IOR = 1" << endl;
          ior[spectrum].resize(scattering[spectrum].size(), complex<double>(1.0, 0.0));
        }
        ior[spectrum].unit = scattering[spectrum].unit;
        ior[spectrum].wavelength = scattering[spectrum].wavelength;
        ior[spectrum].step_size = scattering[spectrum].step_size;      
      }
      else
      {
        cerr << "Warning: Spectral index of refraction and spectral scattering "
             << "coefficient do not have the same number of samples for " << name << "." << endl
             << "  Padding: IOR" << endl;
        Color< complex<double> > old_ior(ior[spectrum]);
        ior[spectrum].resize(scattering[spectrum].size(), old_ior[old_ior.size() - 1]);
        copy(&old_ior[0], &old_ior[old_ior.size()], &ior[spectrum][0]);
      }
    }
    else
    {
      if(scattering[spectrum].size() == 0)
      {
        multi_to_spectrum(scattering, ior[spectrum].size());
        if(scattering[spectrum].size() == 0)
        {
          cerr << "Warning: Scattering coefficient not available for " << name << "." << endl
               << "  Using default: sigma_s = 0" << endl;
          scattering[spectrum].resize(ior[spectrum].size(), 0.0);
        }
        scattering[spectrum].unit = ior[spectrum].unit;
        scattering[spectrum].wavelength = ior[spectrum].wavelength;
        scattering[spectrum].step_size = ior[spectrum].step_size;
      }
      else
      {
        cerr << "Warning: Spectral index of refraction and spectral scattering "
             << "coefficient do not have the same number of samples for " << name << "." << endl
             << "  Padding: sigma_s" << endl;
        Color<double> old_sca(scattering[spectrum]);
        scattering[spectrum].resize(ior[spectrum].size(), old_sca[old_sca.size() - 1]);
        copy(&old_sca[0], &old_sca[old_sca.size()], &scattering[spectrum][0]);
      }
    }
  }
  unsigned int samples = ior[spectrum].size();

  if(samples == 0)
  {
    cerr << "Warning: Spectral data not available for " << name << "." << endl;
    return;
  }

  if(absorption[spectrum].size() != samples)
    absorption[spectrum] = ior_spectrum_to_abs(ior[spectrum]);

  if(turbid)
  {
    if(asymmetry[spectrum].size() != samples)
    {
      if(scattering[spectrum].size() == 0)
      {
        multi_to_spectrum(asymmetry, samples);
        if(asymmetry[spectrum].size() == 0)
        {
          cerr << "Warning: Asymmetry parameter not available for " << name << "." << endl
               << "  Using default: g = 0" << endl;
          asymmetry[spectrum].resize(samples, 0.0);
        }
      }
      else
      {
        Color<double> old_asym(asymmetry[spectrum]);
        asymmetry[spectrum].resize(samples, old_asym[old_asym.size() - 1]);
        copy(&old_asym[0], &old_asym[old_asym.size()], &asymmetry[spectrum][0]);
      }
    }
  }
  else
  {
    if(scattering[spectrum].size() != samples)
      scattering[xyz].resize(samples, 0.0);
    if(asymmetry[spectrum].size() != samples)
      asymmetry[spectrum].resize(samples, 0.0);
  }
  if(extinction[spectrum].size() != samples)
  {
    extinction[spectrum].resize(samples);
    for(unsigned int i = 0; i < extinction[spectrum].size(); ++i)
      extinction[spectrum][i] = scattering[spectrum][i] + absorption[spectrum][i];
  }
  if(albedo[spectrum].size() != samples)
  {
    albedo[spectrum].resize(3);
    for(unsigned int i = 0; i < albedo[spectrum].size(); ++i)
    {
      if(extinction[spectrum][i] == 0.0)
        albedo[spectrum][i] = 0.0;
      else
        albedo[spectrum][i] = scattering[spectrum][i]/extinction[spectrum][i];
    }
  }
  if(reduced_sca[spectrum].size() != samples)
  {
    reduced_sca[spectrum].resize(samples);
    for(unsigned int i = 0; i < reduced_sca[spectrum].size(); ++i)
      reduced_sca[spectrum][i] = scattering[spectrum][i]*(1.0 - asymmetry[spectrum][i]);
  }
  if(reduced_ext[spectrum].size() != samples)
  {
    reduced_ext[spectrum].resize(samples);
    for(unsigned int i = 0; i < reduced_ext[spectrum].size(); ++i)
      reduced_ext[spectrum][i] = reduced_sca[spectrum][i] + absorption[spectrum][i];
  }
  if(reduced_alb[spectrum].size() != samples)
  {
    reduced_alb[spectrum].resize(3);
    for(unsigned int i = 0; i < reduced_alb[spectrum].size(); ++i)
    {
      if(extinction[spectrum][i] == 0.0)
        reduced_alb[spectrum][i] = 0.0;
      else
        reduced_alb[spectrum][i] = reduced_sca[spectrum][i]/reduced_ext[spectrum][i];
    }
  }
}

void Medium::wavelength_to_rgb(double wavelength)
{
  single_to_rgb(wavelength, ior[spectrum], ior[rgb]);
  single_to_rgb(wavelength, absorption[spectrum], absorption[rgb]);

  if(turbid)
  {
    single_to_rgb(wavelength, scattering[spectrum], scattering[rgb]);
    single_to_rgb(wavelength, asymmetry[spectrum], asymmetry[rgb]);
  }
  fill_rgb_data();
}
