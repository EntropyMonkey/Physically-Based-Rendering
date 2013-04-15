// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2013
// Copyright (c) DTU Compute 2013

#include "CGLA/Vec3f.h"
#include "Ray.h"
#include "mt_random.h"
#include "Metal.h"

using namespace CGLA;

Vec3f Metal::shade(Ray& r, bool emit) const
{
  Vec3f result(0.0f);

  // Implement metalic reflection here.
  //
  // Input:  r    (the ray that hit the material)
  //         emit (not used here)
  //
  // Return: radiance reflected to where the ray was coming from
  //
  // Relevant data fields that are available (see Metal.h, Mirror.h, and Ray.h):
  // splits        (number of times to do splitting instead of Russian roulette)
  // tracer        (pointer to ray tracer)
  // r.trace_depth (number of surface interactions previously suffered by the ray)
  //
  // Hint: (a) Use the function tracer->trace_reflected(...) to trace a new ray in
  //       the reflected direction. Retrieve the Fresnel reflectance by supplying 
  //       a third vector argument to this function.
  //       (b) Use the function shade_new_ray(...) to pass the newly traced ray to
  //       the shader for the surface it hit.

  Vec3f radiance = Vec3f(0.0f);

  return result;
}
