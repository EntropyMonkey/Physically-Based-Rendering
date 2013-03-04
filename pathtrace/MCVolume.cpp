// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2013
// Copyright (c) DTU Compute 2013

#include "CGLA/Vec3f.h"
#include "Ray.h"
#include "mt_random.h"
#include "luminance.h"
#include "MCVolume.h"

using namespace CGLA;

Vec3f MCVolume::shade(Ray& r, bool emit) const
{
  r.inside = dot(r.direction, r.hit_normal) > 0.0f;
  const Medium* m = tracer->get_medium(r);
  if(m && m->turbid)
  {
    // Use looping instead of recursion to avoid stack space limit
    Vec3f L(0.0f);
    unsigned int i = static_cast<unsigned int>(mt_random()*3.0); 
    while(i == 3)
      i = static_cast<unsigned int>(mt_random()*3.0);

    Ray rr = r;
    double alb = m->get_albedo(rgb)[i];
    double ext = m->get_extinction(rgb)[i]*scene_scale;
    double g = m->get_asymmetry(rgb)[i];
    do
    {
      // Evaluate the diffusion term using single sample Monte Carlo integration.
      // Importance sample the phase function using tracer->trace_HG(...).
      // Break out of the loop if the ray reaches the surface or gets absorbed (or doesn't hit anything).
      break;
    }
    while(use_multiple_scattering);

    if(!use_multiple_scattering && rr.trace_depth > r.trace_depth)
      L[i] += exp(-ext*rr.dist)*Transparent::shade(rr)[i];

    L[i] *= 3.0f;
    return L;
  }
  return Volume::shade(r, emit);
}
