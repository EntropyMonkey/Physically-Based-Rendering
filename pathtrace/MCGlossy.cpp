// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2013
// Copyright (c) DTU Compute 2013

#include "CGLA/Vec3f.h"
#include "Ray.h"
#include "mt_random.h"
#include "MCGlossy.h"

using namespace CGLA;

Vec3f MCGlossy::shade(Ray& r, bool emit) const
{
  //if(!r.did_hit_diffuse)
  //  return split_shade(r, emit);

  Vec3f rho_d = get_diffuse(r);
  Vec3f rho_s = get_specular(r);
  double luminance_d = get_luminance(rho_d);
  double luminance_s = get_luminance(rho_s);
  Vec3f result = Glossy::shade(r, emit);
  
  // Implement Monte Carlo path tracing for glossy surfaces here.
  //
  // Input:  r    (the ray that hit the material)
  //         emit (passed on to Emission::shade)
  //
  // Return: radiance reflected to where the ray was coming from
  //
  // Relevant data fields that are available (see MCLambertian.h and above):
  // rho_d         (difuse reflectance of the material)
  // rho_s         (specular reflectance of the material)
  // luminance_d   (the probability of diffuse reflection)
  // luminance_s   (the probability of specular reflection)
  // tracer        (pointer to path tracer)
  //
  // Hint: (a) Consider using the function tracer->trace_Blinn_distribution(...) or
  //       tracer->trace_Phong_distribution() or writing a similar function to trace
  //       a new ray in a direction sampled on the hemisphere around the
  //       surface normal according to a sampling function in "sampler.h".
  //       (b) Use the function shade_new_ray(...) to pass the newly traced ray to
  //       the shader for the surface it hit.

  return result;
}

Vec3f MCGlossy::split_shade(Ray& r, bool emit) const
{
  Vec3f rho_d = get_diffuse(r);
  Vec3f rho_s = get_specular(r);
  Vec3f result = Glossy::shade(r, emit);

  // You can optionally split out a number of rays at the first  
  // diffuse surface to get a faster evaluation of the most 
  // important light bounce (corresponds to final gathering)

  return result;
}
