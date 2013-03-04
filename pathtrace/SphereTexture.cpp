// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#include <cmath>
#include "CGLA/Vec3f.h"
#include "CGLA/Vec4f.h"
#include "SphereTexture.h"

using namespace std;
using namespace CGLA;

Vec4f SphereTexture::sample_nearest(const Vec3f& d) const
{
  float u, v;
  project_direction(d, u, v);
  return Texture::sample_nearest(u, v);
}

Vec4f SphereTexture::sample_linear(const Vec3f& d) const
{
  float u, v;
  project_direction(d, u, v);
  return Texture::sample_linear(u, v);
}

void SphereTexture::project_direction(const Vec3f& d, float& u, float& v) const
{
  // Implement the angular map from direction to texture uv-coordinates
}
