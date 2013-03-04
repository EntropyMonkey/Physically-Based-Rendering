// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2013
// Copyright (c) DTU Compute 2013

#include "CGLA/Vec3f.h"
#include "../optprops/Medium.h"
#include "Ray.h"
#include "mt_random.h"
#include "luminance.h"
#include "Volume.h"

using namespace CGLA;

const double Volume::scene_scale = 1.0e-2; // m -> cm

Vec3f Volume::shade(Ray& r, bool emit) const
{
  r.inside = dot(r.direction, r.hit_normal) > 0.0f;
  if(r.trace_depth < splits + 1) // attenuation happens after second bounce
    return split_shade(r, emit);

  const Medium* m = tracer->get_medium(r);
  if(m)
  {
    // Compute direct transmission using Russian roulette with the transmittance
    // to let absorption stop the recursion.

  }
  return Transparent::shade(r, emit);
}

Vec3f Volume::split_shade(Ray& r, bool emit) const
{
  // Find the direct transmission through the volume by using the transmittance
  // to modify the result from the Transparent shader.
  const Medium* m = tracer->get_medium(r);
  return get_transmittance(r, m)*Transparent::shade(r, emit);
}

Vec3f Volume::get_transmittance(Ray& r, const Medium* m) const
{
  if(m)
  {
    // Compute and return the transmittance using the rgb extinction coefficient of the medium.
    // The Medium class holds scattering properties in SI units. As most scenes are modeled in 
    // centimeters rather than meters, transform the extinction coefficient: 1/m -> 1/cm.

  }
  return Vec3f(1.0f);
}

double Volume::get_luminance(const Vec3f& color) const
{
  return get_luminance_NTSC(color[0], color[1], color[2]);  
}
