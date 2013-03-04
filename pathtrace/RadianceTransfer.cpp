// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2013
// Copyright (c) DTU Compute 2013

#include <cmath>
#include <valarray>
#include "CGLA/Vec3f.h"
#include "CGLA/Vec2d.h"
#include "Ray.h"
#include "sampler.h"
#include "legendre.h"
#include "RadianceTransfer.h"

using namespace std;
using namespace CGLA;

namespace
{
  const double M_2PI = 2.0*M_PI;
  const double M_4PI = 4.0*M_PI;

  Vec2d vec_to_spherical(Vec3f v)
  {
    double cos_theta = v[2];
    double phi = atan2(v[1], v[0]);
    return Vec2d(cos_theta, phi < 0.0 ? phi + M_2PI : phi);
  }
}

Vec3f RadianceTransfer::shade(Ray& r, bool emit) const
{
  if(r.trace_depth > 0)
    return MCLambertian::shade(r, emit);

  valarray<Vec3f> coeffs(Vec3f(0.0f), bands*bands);

  // Compute spherical harmonics coefficients 
  // (the basis functions are available in legendre.h)

  // Save coefficients to file
  if(fout && (*fout).is_open())
  {
    for(unsigned int i = 0; i < coeffs.size(); ++i)
      fout->write(reinterpret_cast<const char*>(coeffs[i].get()), sizeof(Vec3f));
  }

  // Reconstruct lighting for preview
  Vec3f result(0.0f);

  return result;
}
