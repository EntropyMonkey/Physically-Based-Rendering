// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2013
// Copyright (c) DTU Compute 2013

#include "CGLA/Vec3f.h"
#include "Ray.h"
#include "Glossy.h"

using namespace CGLA;

#ifndef M_1_PI
#define M_1_PI 0.31830988618379067154
#endif

Vec3f Glossy::shade(Ray& r, bool emit) const
{
  Vec3f rho_d = get_diffuse(r);
  Vec3f rho_s = get_specular(r);
  float s = get_shininess(r);
  Vec3f result(0.0f);

  // Implement glossy reflection here.
  //
  // Input:  r    (the ray that hit the material)
  //         emit (passed on to Emission::shade)
  //
  // Return: radiance reflected to where the ray was coming from
  //
  // Relevant data fields that are available (see Lambertian.h, Ray.h, and above):
  // lights        (vector of pointers to the lights in the scene)
  // r.hit_pos     (surface position where the ray hit the material)
  // r.hit_normal  (surface normal where the ray hit the material)
  // rho_d         (difuse reflectance of the material)
  // rho_s         (specular reflectance of the material)
  // s             (shininess of the material)
  //
  // Hint: (a) Call the sample function associated with each light in the scene.
  //       (b) If you choose to implement the Phong or the Blinn model, I suggest
  //       that you convert shininess to an integer and use the function int_pow(...)
  //       which is much more efficient than the general pow(...) function.

  return result + Emission::shade(r, emit);
}
