// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2013
// Copyright (c) DTU Compute 2013

#include "CGLA/Vec3f.h"
#include "Ray.h"
#include "Mirror.h"

using namespace CGLA;

Vec3f Mirror::shade(Ray& r, bool emit) const
{
  // Implement mirror reflection here.
  //
  // Input:  r    (the ray that hit the material)
  //         emit (not used here)
  //
  // Return: radiance reflected to where the ray was coming from
  //
  // Relevant data fields that are available (see Mirror.h and Ray.h):
  // max_depth     (maximum trace depth)
  // tracer        (pointer to ray tracer)
  // r.trace_depth (number of surface interactions previously suffered by the ray)
  //
  // Hint: (a) Use the function tracer->trace_reflected(...) to trace a new ray in
  //       the reflected direction.
  //       (b) Use the function shade_new_ray(...) to pass the newly traced ray to
  //       the shader for the surface it hit.

  return Vec3f(0.0f);
}

Vec3f Mirror::shade_new_ray(Ray& r) const
{
  if(r.has_hit)
  {
    const Shader* s = tracer->get_shader(r);
    if(s)
      return s->shade(r, true);
  }
  else
    return tracer->get_background(r.direction);  

  return Vec3f(0.0f);
}
