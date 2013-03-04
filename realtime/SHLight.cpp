// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#include "CGLA/Vec3f.h"
#include "CGLA/Vec2d.h"
#include "SHLight.h"

using namespace CGLA;

namespace
{
  const double M_2PI = 2.0*M_PI;
}

void SHLight::upload_sh(GLhandleARB program) const
{
  // Upload spherical harmonics coefficients to shaders
  glUniform3fv(glGetUniformLocation(program, "light"), coeffs.size(), coeffs[0].get());
}

Vec2d SHLight::vec_to_spherical(Vec3f v)
{
  double cos_theta = v[2];
  double phi = atan2(v[1], v[0]);
  return Vec2d(cos_theta, phi < 0.0 ? phi + M_2PI : phi);
}
