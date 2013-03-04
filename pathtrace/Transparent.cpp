// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2013
// Copyright (c) DTU Compute 2013

#include "CGLA/Vec3f.h"
#include "Ray.h"
#include "mt_random.h"
#include "Transparent.h"

using namespace CGLA;

Vec3f Transparent::shade(Ray& r, bool emit) const
{
  Vec3f result = split_shade(r, emit);

  // Implement reflection and refraction using Russian roulette.
  //
  // Input:  r    (the ray that hit the material)
  //         emit (not used here)
  //
  // Return: radiance reflected or refracted to where the ray was coming from
  //
  // Relevant data fields that are available (see Transparent.h, Mirror.h, and Ray.h):
  // splits        (number of times to do splitting instead of Russian roulette)
  // tracer        (pointer to ray tracer)
  // r.trace_depth (number of surface interactions previously suffered by the ray)
  //
  // Hint: (a) Use the functions tracer->trace_reflected(...) and tracer->trace_refracted(...)
  //       to trace a new ray in the reflected and refracted directions. Retrieve the
  //       Fresnel reflectance by supplying a third argument to one of these functions.
  //       (b) Use the function shade_new_ray(...) to pass the newly traced ray to
  //       the shader for the surface it hit.

  return result;
}

Vec3f Transparent::split_shade(Ray& r, bool emit) const
{
  // Implement reflection and refraction using splitting.
  //
  // Input:  r    (the ray that hit the material)
  //         emit (not used here)
  //
  // Return: radiance reflected and refracted to where the ray was coming from
  //
  // Relevant data fields that are available (see Mirror.h and Ray.h):
  // max_depth     (maximum trace depth)
  // tracer        (pointer to ray tracer)
  // r.trace_depth (number of surface interactions previously suffered by the ray)
  //
  // Hint: (a) Use the functions tracer->trace_reflected(...) and tracer->trace_refracted(...)
  //       to trace a new ray in the reflected and refracted directions. Retrieve the
  //       Fresnel reflectance by supplying a third argument to one of these functions.
  //       (b) Use the function shade_new_ray(...) to pass a newly traced ray to
  //       the shader for the surface it hit.

  return Vec3f(0.0f);
}
