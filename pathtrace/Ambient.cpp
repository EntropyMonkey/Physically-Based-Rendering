// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2013
// Copyright (c) DTU Compute 2013

#include "CGLA/Vec3f.h"
#include "Ray.h"
#include "Ambient.h"

using namespace CGLA;
using namespace std;

Vec3f Ambient::shade(Ray& r, bool emit) const
{ 
  // Implement ambient occlusion here.
  //
  // Input:  r    (the ray that hit the material)
  //         emit (passed on to Emission::shade)
  //
  // Return: radiance reflected to where the ray was coming from
  //
  // Relevant data fields that are available (see Ambient.h):
  // samples  (number of times to trace a sample ray)
  // tracer   (pointer to ray tracer)
  //
  // Hint: (a) Use the function tracer->trace_cosine_weighted(...) to trace
  //       a new ray in a direction sampled on the hemisphere around the
  //       surface normal according to the function sample_cosine_weighted(...).
  //       (b) Use the function tracer->get_background(...) to retrieve
  //       the ambient light in the direction of an unoccluded ray.

  Vec3f rho_d = get_diffuse(r);
  Vec3f radiance(0.0f);

  for (int sample = 0; sample < samples; sample++)
  {
    Ray ray;

    bool inShadow = tracer->trace_cosine_weighted(r, ray);

    if (!inShadow)
    {
      Vec3f sampleRadiance = tracer->get_background(ray.direction);
      radiance += sampleRadiance * dot(r.hit_normal, ray.direction);
    }
  }
  radiance *= rho_d / samples;

  radiance += Lambertian::shade(r, emit);

  return radiance;
}
