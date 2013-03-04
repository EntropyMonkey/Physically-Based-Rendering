// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2013
// Copyright (c) DTU Compute 2013

#include "CGLA/Vec3f.h"
#include "Ray.h"
#include "Lambertian.h"

using namespace CGLA;

// The following macro defines 1/PI
#ifndef M_1_PIf
#define M_1_PIf 0.31830988618379067154f
#endif

Vec3f Lambertian::shade(Ray& r, bool emit) const
{
  Vec3f rho_d = get_diffuse(r);
  Vec3f result(0.0f);
  
  // Implement Lambertian reflection here.
  //
  // Input:  r    (the ray that hit the material)
  //         emit (passed on to Emission::shade)
  //
  // Return: radiance reflected to where the ray was coming from
  //
  // Relevant data fields that are available (see Lambertian.h, Ray.h, and above):
  // lights        (vector of pointers to the lights in the scene)
  // r.hit_pos     (position where the ray hit the material)
  // r.hit_normal  (surface normal where the ray hit the material)
  // rho_d         (difuse reflectance of the material)
  //
  // Hint: (a) Call the sample function associated with each light in the scene.
  //       (b) The framework includes a function dot(v, w) to take the dot product
  //       of two vectors v and w.
  
  // temp light direction and radiance
  Vec3f lightDirection, radiance;
  for (std::vector<Light*>::const_iterator it = lights.begin(); it != lights.end(); it++)
  {
    if ((*it)->sample(r.hit_pos, lightDirection, radiance))
    {
      // output of Lambertian BRDF
      Vec3f f = rho_d * M_1_PIf;
      
      // directional light radiance
      // f - scattered light radiance, radiance - current light radiance, last term: cosine cut off at 0
      result += f * radiance * std::max(dot(r.hit_normal, lightDirection), 0.0f);
    }
  }

  return result + Emission::shade(r, emit);
}
