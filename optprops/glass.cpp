// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2010
// Copyright (c) DTU Informatics 2010

#include <cmath>
#include <complex>
#include "Medium.h"
#include "glass.h"

using namespace std;

namespace
{
  const unsigned int no_of_samples = 17;
  const double first_wavelength = 375.0;
  const double wavelength_step_size = 25.0;

  const double schott_C[][6] = { { 2.1573978, 8.4004189e-3, 1.0457582e-2, 2.1822593e-4, 5.5063640e-6, 5.4469060e-7 },    // deep crown (code 479587 TiK1)
                                 { 2.2850299, 8.6010725e-3, 1.1806783e-2, 2.0765657e-4, 2.1314913e-6, 3.2131234e-7 },    // crown (code 522595 K5)
                                 { 2.2824396, 8.5960144e-3, 1.3442645e-2, 2.7803535e-4, 4.9998960e-7, 7.7105911e-7 },    // crown flint (code 523515 KF9)
                                 { 2.4441760, 8.3059695e-3, 1.9000697e-2, 5.4129697e-4, 4.1973155e-6, 2.3742897e-6 },    // light flint (code 581409 LF5)
                                 { 2.5554063, 8.8746150e-3, 2.2494787e-2, 8.6924972e-4, 2.4011704e-5, 4.5365169e-6 },    // flint (code 620364 F2)
                                 { 2.6531250, 8.1388553e-3, 2.2995643e-2, 7.3535957e-4, 1.3407390e-5, 3.6962325e-6 },    // dense barium flint (code 650392 BaSF10)
                                 { 3.1195007, 1.0902580e-2, 4.1330651e-2, 3.1800214e-3, 2.1953184e-4, 2.6671014e-5 }  }; // dense flint (code 805254 SF6)

  enum SchottName { deep_crown, crown, crown_flint, light_flint, flint, dense_barium_flint, dense_flint };

  // Formulae for Schott glasses [Tropf et al. 1995, Schott Glass Technologies]
  Medium schott_glass(SchottName n)
  {
    Medium m;
    Color< complex<double> >& ior = m.get_ior(spectrum);
    ior.resize(no_of_samples);
    ior.wavelength = first_wavelength;
    ior.step_size = wavelength_step_size;

    for(unsigned int i = 0; i < ior.size(); ++i)
    {
      double lambda = (ior.wavelength + i*ior.step_size)*1.0e-3; // wavelength in micrometers
      double lambda2 = lambda*lambda;
      double inv_lambda2 = 1.0/lambda2;
      double inv_lambda4 = inv_lambda2*inv_lambda2;

      double n2 = schott_C[n][0] - schott_C[n][1]*lambda2 + schott_C[n][2]*inv_lambda2
                  + schott_C[n][3]*inv_lambda4 - schott_C[n][4]*inv_lambda2*inv_lambda4 + schott_C[n][5]*inv_lambda4*inv_lambda4;

      ior[i] = complex<double>(sqrt(n2), 0.0);
    }
    return m;    
  }
}

Medium deep_crown_glass()
{
  Medium m = schott_glass(deep_crown);
  m.name = "deep_crown_glass";
  return m;
}

Medium crown_glass()
{
  Medium m = schott_glass(crown);
  m.name = "crown_glass";
  return m;
}

Medium crown_flint_glass()
{
  Medium m = schott_glass(crown_flint);
  m.name = "crown_flint_glass";
  return m;
}

Medium light_flint_glass()
{
  Medium m = schott_glass(light_flint);
  m.name = "light_flint_glass";
  return m;
}

Medium dense_barium_flint_glass()
{
  Medium m = schott_glass(dense_barium_flint);
  m.name = "dense_barium_flint_glass";
  return m;
}

Medium dense_flint_glass()
{
  Medium m = schott_glass(dense_flint);
  m.name = "dense_flint_glass";
  return m;
}
