// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2013
// Copyright (c) DTU Compute 2013

#include <cmath>
#include "CGLA/Vec3f.h"
#include "Ray.h"
#include "sampler.h"
#include "PointLight.h"

using namespace std;
using namespace CGLA;

bool PointLight::sample(const Vec3f& pos, Vec3f& dir, Vec3f& L) const
{
  // Set dir and L and return true if not in shadow

  return false;
}

bool PointLight::emit(Ray& r, Vec3f& Phi) const
{
  // Sample ray origin and direction
  
  // Trace ray
  
  // If a surface was hit, compute Phi and return true

  return false;
}
