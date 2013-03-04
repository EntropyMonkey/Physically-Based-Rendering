// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2013
// Copyright (c) DTU Compute 2013

#include "CGLA/Vec3f.h"
#include "Ray.h"
#include "PhotonCaustics.h"

using namespace CGLA;

Vec3f PhotonCaustics::shade(Ray& r, bool emit) const
{
  Vec3f rho_d = get_diffuse(r);
  Vec3f result(0.0f);

  // Make a radiance estimate using the caustics photon map
  //
  // Input:  r    (the ray that hit the material)
  //         emit (passed on to Emission::shade)
  //
  // Return: radiance reflected to where the ray was coming from
  //
  // Relevant data fields that are available (see PhotonCaustics.h and above):
  // rho_d     (difuse reflectance of the material)
  // tracer    (pointer to particle tracer)
  // max_dist  (maximum radius of radiance estimate)
  // photons   (maximum number of photons to be included in radiance estimate)
  //
  // Hint: Use the function tracer->caustics_irradiance(...) to do an
  //       irradiance estimate using the photon map. This is not the 
  //       same as a radiance estimate.

  return result + Emission::shade(r, emit);
}
