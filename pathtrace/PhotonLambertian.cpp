// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2013
// Copyright (c) DTU Compute 2013

#include "CGLA/Vec3f.h"
#include "Ray.h"
#include "PhotonLambertian.h"

using namespace CGLA;

Vec3f PhotonLambertian::shade(Ray& r, bool emit) const
{
  if(gather && !r.did_hit_diffuse)
    return split_shade(r, emit);

  Vec3f rho_d = get_diffuse(r);
  Vec3f result(0.0f);

  // Make a radiance estimate using the global photon map.
  //
  // Input:  r    (the ray that hit the material)
  //         emit (not used here unless passed on to split_shade)
  //
  // Return: radiance reflected to where the ray was coming from
  //
  // Relevant data fields that are available (see PhotonLambertian.h, 
  // PhotonCaustics.h, and above):
  // rho_d     (difuse reflectance of the material)
  // tracer    (pointer to particle tracer)
  // max_dist  (maximum radius of radiance estimate)
  // photons   (maximum number of photons to be included in radiance estimate)
  //
  // Hint: Use the function tracer->global_irradiance(...) to do an
  //       irradiance estimate using the photon map. This is not the 
  //       same as a radiance estimate.

  return result + Emission::shade(r, !gather);
}

Vec3f PhotonLambertian::split_shade(Ray& r, bool emit) const
{
  Vec3f rho_d = get_diffuse(r); 
  Vec3f result = Lambertian::shade(r, emit);
  if(rho_d[0] + rho_d[1] + rho_d[2] > 0.0)
  {
    // Do final gathering here.
    //
    // Input:  r    (the ray that hit the material)
    //         emit (passed on to Lambertian::shade)
    //
    // Return: radiance reflected to where the ray was coming from
    //
    // Relevant data fields that are available (see PhotonLambertian.h, 
    // PhotonCaustics.h, and above):
    // rho_d      (difuse reflectance of the material)
    // tracer   (pointer to particle tracer)
    // samples  (number of samples to be traced when splitting)
    //
    // Hint: (a) Use the function tracer->trace_cosine_weighted(...) to trace
    //       a new ray in a direction sampled on the hemisphere around the
    //       surface normal according to the function sample_cosine_weighted(...).
    //       (b) Use the function shade_new_ray(...) to pass the newly traced ray to
    //       the shader for the surface it hit.
  
    // Add a radiance estimate from the caustics photon map
    result += caustics 
      ? caustics->shade(r, false)
      : PhotonCaustics::shade(r, false);
  }
  return result;
}

Vec3f PhotonLambertian::shade_new_ray(Ray& r) const
{
  if(r.has_hit)
  {
    r.did_hit_diffuse = true;
    const Shader* s = tracer->get_shader(r);
    if(s)
      return s->shade(r, false);
  }
  else
    return tracer->get_background(r.direction);  

  return Vec3f(0.0f);
}

