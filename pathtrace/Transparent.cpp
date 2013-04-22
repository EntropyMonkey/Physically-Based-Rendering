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
  
  Vec3f radiance = Vec3f(0.0f);

  if (r.trace_depth < splits)
  {
    radiance = split_shade(r, emit);
  }
  else if (r.trace_depth < max_depth)
  {
    // refraction
    Ray refracted;
    double fresnelR;
    tracer->trace_refracted(r, refracted, fresnelR); // fresnelR => use as step probability

    // russian roulette for reflections
    float rand = randomizer.mt_random();

    // 1st cond. -> russian roulette with fresnelR => pdf, 2nd cond. -> eliminating rays following surface
    if (rand <= fresnelR && fresnelR > 0.001)
    {
      // reflect
      Ray reflected;
      tracer->trace_reflected(r, reflected);
      radiance = shade_new_ray(reflected); // * fresnelR / fresnelR; // divide by fresnelR, since fresnelR is used as the step probability
    }
    // if not reflecting, take refraction
    else if (1 - fresnelR > 0.001)
    {
      radiance = shade_new_ray(refracted); // * (1 - fresnelR) / (1 - fresnelR);
    }
  }

  return radiance;
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

  Vec3f radiance(0.0f);

  if (r.trace_depth < splits)
  {
    Ray refracted;
    double fresnelR;
    tracer->trace_refracted(r, refracted, fresnelR);

    if (1 - fresnelR > 0.001)
      radiance += shade_new_ray(refracted) * (1.0f - fresnelR);

    // eliminate rays following the surface
    if (fresnelR > 0.001)
    {
      Ray reflected;
      tracer->trace_reflected(r, reflected);
      radiance += shade_new_ray(reflected) * fresnelR;
    }
  }


  return radiance;
}
