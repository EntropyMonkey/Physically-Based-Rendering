// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#include <valarray>
#include "CGLA/Vec3f.h"
#include "CGLA/Vec2d.h"
#include "legendre.h"
#include "mt_random.h"
#include "SHOmni.h"

using namespace std;
using namespace CGLA;

namespace
{
  const double M_4PI = 4.0*M_PI;
}

void SHOmni::compute_sh()
{
  // Compute spherical harmonics coefficients for omnidirectional light
}

Vec3f SHOmni::sample_sphere() const
{
  // Use rejection sampling to find an arbitrary direction
  return Vec3f(0.0f);
}
