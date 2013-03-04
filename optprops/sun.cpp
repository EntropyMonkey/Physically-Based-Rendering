#include <cmath>
#include <complex>
#include "CGLA/Vec3f.h"
#include "../pathtrace/cdf_bsearch.h"
#include "../pathtrace/sampler.h"
#include "sun.h"

using namespace std;
using namespace CGLA;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace
{
  const unsigned int table_size = 122;
  const unsigned int no_of_samples = 22;
  const double first_wavelength = 390.0;
  const double wavelength_step_size = 20.0;

  /* Data from Bird and Riordan [US Department of Energy, 1984].
     This array contains the extraterrestrial spectrum and atmospheric 
	   absorption coefficients at 122 wavelengths.  The first array range is
	   defined as follows:
	     0 = wavelength (microns)
	     1 = extraterrestrial spectrum (W/sq m/micron)
	     2 = water vapor absorption coefficient
	     3 = ozone absorption coefficient
	     4 = uniformly mixed gas "absorption coefficient"   */
  double A[5][122] = { { 0.3, 0.305, 0.31, 0.315, 0.32, 0.325, 0.33, 0.335, 0.34,
                         0.345, 0.35, 0.36, 0.37, 0.38, 0.39, 0.4, 0.41, 0.42, 0.43, 0.44, 0.45, 0.46, 0.47,
                         0.48, 0.49, 0.5, 0.51, 0.52, 0.53, 0.54, 0.55, 0.57, 0.593, 0.61, 0.63, 0.656, 
                         0.6676, 0.69, 0.71, 0.718, 0.7244, 0.74, 0.7525, 0.7575, 0.7625, 0.7675, 0.78, 0.8,
                         0.816, 0.8237, 0.8315, 0.84, 0.86, 0.88, 0.905, 0.915, 0.925, 0.93, 0.937, 0.948,
                         0.965, 0.98, 0.9935, 1.04, 1.07, 1.1, 1.12, 1.13, 1.145, 1.161, 1.17, 1.2, 1.24, 
                         1.27, 1.29, 1.32, 1.35, 1.395, 1.4425, 1.4625, 1.477, 1.497, 1.52, 1.539, 1.558,
                         1.578, 1.592, 1.61, 1.63, 1.646, 1.678, 1.74, 1.8, 1.86, 1.92, 1.96, 1.985, 2.005,
                         2.035, 2.065, 2.1, 2.148, 2.198, 2.27, 2.36, 2.45, 2.5, 2.6, 2.7, 2.8, 2.9, 3.0, 
                         3.1, 3.2, 3.3, 3.4, 3.5, 3.6, 3.7, 3.8, 3.9, 4.0 },
                       { 535.9, 558.3, 622.0, 692.7, 715.1, 832.9, 961.9, 931.9, 900.6, 911.3, 975.5,
                         975.9, 1119.9, 1103.8, 1033.8, 1479.1, 1701.3, 1740.4, 1587.2, 1837.0, 2005.0,
                         2043.0, 1987.0, 2027.0, 1896.0, 1909.0, 1927.0, 1831.0, 1891.0, 1898.0, 1892.0,
                         1840.0, 1768.0, 1728.0, 1658.0, 1524.0, 1531.0, 1420.0, 1399.0, 1374.0, 1373.0,
                         1298.0, 1269.0, 1245.0, 1223.0, 1205.0, 1183.0, 1148.0, 1091.0, 1062.0, 1038.0,
                         1022.0, 998.7, 947.2, 893.2, 868.2, 829.7, 830.3, 814.0, 786.9, 768.3, 767.0, 757.6,
                         688.1, 640.7, 606.2, 585.9, 570.2, 564.1, 544.2, 533.4, 501.6, 477.5, 442.7, 440.0,
                         416.8, 391.4, 358.9, 327.5, 317.5, 307.3, 300.4, 292.8, 275.5, 272.1, 259.3, 246.9,
                         244.0, 243.5, 234.8, 220.5, 190.8, 171.1, 144.5, 135.7, 123.0, 123.8, 113.0, 108.5,
                         97.5, 92.4, 82.4, 74.6, 68.3, 63.8, 49.5, 48.5, 38.6, 36.6, 32.0, 28.1, 24.8, 22.1,
                         19.6, 17.5, 15.7, 14.1, 12.7, 11.5, 10.4, 9.5, 8.6 },
                       { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                         0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                         0.075, 0.0, 0.0, 0.0, 0.0, 0.016, 0.0125, 1.8, 2.5, 0.061, 0.0008, 0.0001, 0.00001,
                         0.00001, 0.0006, 0.036, 1.6, 2.5, 0.5, 0.155, 0.00001, 0.0026, 7.0, 5.0, 5.0, 27.0,
                         55.0, 45.0, 4.0, 1.48, 0.1, 0.00001, 0.001, 3.2, 115.0, 70.0, 75.0, 10.0, 5.0, 2.0,
                         0.002, 0.002, 0.1, 4.0, 200.0, 1000.0, 185.0, 80.0, 80.0, 12.0, 0.16, 0.002, 0.0005,
                         0.0001, 0.00001, 0.0001, 0.001, 0.01, 0.036, 1.1, 130.0, 1000.0, 500.0, 100.0, 4.0,
                         2.9, 1.0, 0.4, 0.22, 0.25, 0.33, 0.5, 4.0, 80.0, 310.0, 15000.0, 22000.0, 8000.0,
                         650.0, 240.0, 230.0, 100.0, 120.0, 19.5, 3.6, 3.1, 2.5, 1.4, 0.17, 0.0045 },
                       { 10.0, 4.8, 2.7, 1.35, 0.8, 0.38, 0.16, 0.075, 0.04, 0.019, 0.007, 0.0, 0.0, 0.0,
                         0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.003, 0.006, 0.009, 0.01400, 0.021, 0.03, 0.04,
                         0.048, 0.063, 0.075, 0.085, 0.12, 0.119, 0.12, 0.09, 0.065, 0.051, 0.028, 0.018,
                         0.015, 0.012, 0.01, 0.008, 0.007, 0.006, 0.005, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                         0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                         0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                         0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                         0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                         0.0 },
                       { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                         0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                         0.0, 0.0, 0.0, 0.0, 0.15, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 4.0, 0.35, 0.0, 0.0, 0.0,
                         0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                         0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.05, 0.3, 0.02, 0.0002, 0.00011, 0.00001, 0.05,
                         0.011, 0.005, 0.0006, 0.0, 0.005, 0.13, 0.04, 0.06, 0.13, 0.001, 0.0014, 0.0001,
                         0.00001, 0.00001, 0.0001, 0.001, 4.3, 0.2, 21.0, 0.13, 1.0, 0.08, 0.001, 0.00038,
                         0.001, 0.0005, 0.00015, 0.00014, 0.00066, 100.0, 150.0, 0.13, 0.0095, 0.001, 0.8,
                         1.9, 1.3, 0.075, 0.01, 0.00195, 0.004, 0.29, 0.025 } };

  // Approximations for Earth-Sun distance correction factor and solar declination [Spencer 1971]
  void spencer(double day, double& earth_sun_dist, double& declination)
  {
    double day_angle = (day - 1.0)*2.0*M_PI/365.0;
    double cosA = cos(day_angle);
    double sinA = sin(day_angle);
    double cosA_sqr = cosA*cosA; 
    double sinA_sqr = sinA*sinA;
    double cos2A = cosA_sqr - sinA_sqr;
    double sin2A = 2.0*sinA*cosA;
    double cos3A = (cosA_sqr - 3.0*sinA_sqr)*cosA;
    double sin3A = (3.0*cosA_sqr - sinA_sqr)*sinA;
    earth_sun_dist = 1.00011 + 0.034221*cosA + 0.001280*sinA + 0.000719*cos2A + 0.000077*sin2A;
    declination = 0.006918 - 0.399912*cosA + 0.070257*sinA - 0.006758*cos2A + 0.000907*sin2A - 0.002697*cos3A + 0.001480*sin3A;
  }

  // Common equations for computing the sun position [Preetham et al. 1999, Prein and Gayanilo 1992, Milankovitch 1930]
  inline Vec3f solar_elevation(double time, double latitude, double declination)
  {
    double hour_angle = (time - 12.0)*M_PI/12.0;
    double cos_hour_angle = cos(hour_angle);
    double sin_hour_angle = sign(hour_angle)*sqrt(1.0 - cos_hour_angle*cos_hour_angle);
    double cos_latitude = cos(latitude);
    double sin_latitude = sin(latitude);
    double cos_declination = cos(declination);
    double sin_declination = sin(declination);
    double cos_zenith = sin_latitude*sin_declination + cos_latitude*cos_declination*cos_hour_angle;
    double sin_zenith = sqrt(1.0 - cos_zenith*cos_zenith); //take care of sign!
    double tan_phi = cos_declination*sin_hour_angle/(cos_latitude*sin_declination + sin_latitude*cos_declination*cos_hour_angle);
    double phi = atan(tan_phi);
    return spherical_direction(sin_zenith, cos_zenith, phi);
  }
}

// Based on tabulated measurements of mean extraterrestrial solar irradiance
Medium mean_solar_irrad()
{
  Medium m;
  Color< complex<double> >& ior = m.get_ior(mono);
  ior.resize(1, complex<double>(1.0, 0.0));
  Color<double>& emission = m.get_emission(spectrum);
  emission.resize(no_of_samples);
  emission.wavelength = first_wavelength;
  emission.step_size = wavelength_step_size;

  // Use look-up table and linear interpolation to fill array
  for(unsigned int i = 0; i < emission.size(); ++i)
  {
    double lambda = (emission.wavelength + i*emission.step_size)*1.0e-3; // wavelength [{\mu}m]
    unsigned int idx = cdf_bsearch(lambda, A[0], table_size) - 1;
    double pos = (lambda - A[0][idx])/(A[0][idx + 1] - A[0][idx]);
    emission[i] = (A[1][idx]*(1.0 - pos) + A[1][idx + 1]*pos)*1.0e-3;    // per nanometer
  }
  m.name = "sun";
  m.emissive = true;
  return m;
}

// Placing the sun approximately using the following arguments
// day       - day in the year counting from 1 January
// time      - hour in the day where 12 is solar noon
// latitude  - angular distance in degrees measured from equator
// up        - the world's up direction
// direction - direction toward the sun (output)
Medium solar_irrad(double day, double time, double latitude, const Vec3f& up, Vec3f& direction)
{
  // Establish the direction toward the sun
  double earth_sun_dist, declination;
  spencer(day, earth_sun_dist, declination);
  direction = solar_elevation(time, latitude*M_PI/180.0, declination);
  rotate_to_normal(up, direction);

  // Compute the spectrum of the solar radiation
  Medium m = mean_solar_irrad();
  Color<double>& emission = m.get_emission(spectrum);
  for(unsigned int i = 0; i < emission.size(); ++i)
    emission[i] *= earth_sun_dist;
  return m;
}

Medium direct_sun(double day, double time, double latitude, const Vec3f& up, Vec3f& direction)
{
  Medium m = solar_irrad(day, time, latitude, up, direction);
  return m;
}

Medium atmosphere(double day, double time, double latitude, const Vec3f& up, Vec3f& direction)
{
  Medium m = solar_irrad(day, time, latitude, up, direction);
  m.name = "atmosphere";
  return m;
}
