// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2013
// Copyright (c) DTU Compute 2013

#include "CGLA/Vec3f.h"
#include "Ray.h"
#include "mt_random.h"
#include "luminance.h"
#include "MCLambertian.h"

using namespace CGLA;

Vec3f MCLambertian::shade(Ray& r, bool emit) const
{
  if(!r.did_hit_diffuse)
    return split_shade(r, emit);

  Vec3f rho_d = get_diffuse(r);
  double luminance = get_luminance(rho_d);
  Vec3f result(0.0f);
  
  // Implement Monte Carlo path tracing for Lambertian surfaces here.
  //
  // Input:  r    (the ray that hit the material)
  //         emit (passed on to Lambertian::shade)
  //
  // Return: radiance reflected to where the ray was coming from
  //
  // Relevant data fields that are available (see MCLambertian.h and above):
  // rho_d     (difuse reflectance of the material)
  // luminance (the probability of diffuse reflection vs. absorption)
  // tracer    (pointer to path tracer)
  //
  // Hint: (a) Use the function tracer->trace_cosine_weighted(...) to trace
  //       a new ray in a direction sampled on the hemisphere around the
  //       surface normal according to the function sample_cosine_weighted(...).
  //       (b) Use the function shade_new_ray(...) to pass the newly traced ray to
  //       the shader for the surface it hit.
  
  float rand = mt_random();

  // the more light is being reflected by a surface, the higher the probability for reflection should be.
  // also: weight different diffuse colors depending on human eye sensitivity
  float y = 0.2989 * rho_d[0] + 0.5866 * rho_d[1] + 0.1145 * rho_d[2];
  //float y = (rho_d[0] + rho_d[1] + rho_d[2]) / 3.0f;
  
  // reference: http://www.youtube.com/watch?v=xIPKmbuVHQI
  if (rand < y)
  {
    // direct light
    result += Lambertian::shade(r, emit) / y; // divide by probability to make sure that monte carlo gives valid results

    // indirect light
    for (unsigned int sample = 0; sample < samples; sample++)
    { 
      Ray hR; 
      tracer->trace_cosine_weighted(r, hR); // trace diffuse light in a hemisphere
      result += shade_new_ray(hR) * dot(r.hit_normal, hR.direction) / y / samples;
    }
    
    // f -> BRDF for Lambertian surfaces
    Vec3f f = rho_d / M_PI;
    result *= f; // average sampled radiances
  }
  else // not tracing this ray any further
  {
    result += Lambertian::shade(r, emit);
  }

  return result;
}

Vec3f MCLambertian::split_shade(Ray& r, bool emit) const
{
  Vec3f rho_d = get_diffuse(r);
  Vec3f result(0.0f);

  // Split out a number of rays at the first diffuse surface 
  // to get a faster evaluation of the most important light bounce
  // (corresponds to final gathering).
  //
  // Input:  r    (the ray that hit the material)
  //         emit (passed on to Lambertian::shade)
  //
  // Return: radiance reflected to where the ray was coming from
  //
  // Relevant data fields that are available (see MCLambertian.h and above):
  // rho_d   (difuse reflectance of the material)
  // samples (number of samples to be traced when splitting)
  // tracer  (pointer to path tracer)
  //
  // Hint: (a) Use the function tracer->trace_cosine_weighted(...) to trace
  //       a new ray in a direction sampled on the hemisphere around the
  //       surface normal according to the function sample_cosine_weighted(...).
  //       (b) Use the function shade_new_ray(...) to pass the newly traced ray to
  //       the shader for the surface it hit.

  // indirect light
  for (unsigned int sample = 0; sample < samples; sample++)
  { 
    Ray hR; 
    tracer->trace_cosine_weighted(r, hR); // trace diffuse light in a hemisphere
    result += shade_new_ray(hR) * dot(r.hit_normal, hR.direction);
  }
     
  // f -> BRDF for Lambertian surfaces
  Vec3f f = rho_d / M_PI;
  result *= f / samples; // average sampled radiances

  result += Lambertian::shade(r, emit); // direct light

  return result;
}

Vec3f MCLambertian::shade_new_ray(Ray& r) const
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

// Using luminance to importance sample according to eye sensitivity
double MCLambertian::get_luminance(const Vec3f& color) const
{
  return get_luminance_NTSC(color[0], color[1], color[2]);  
}
