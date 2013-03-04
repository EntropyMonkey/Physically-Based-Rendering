// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2013
// Copyright (c) DTU Compute 2013

#include <sstream>
#include <string>
#include "CGLA/Vec3f.h"
#include "Ray.h"
#include "sampler.h"
#include "Directional.h"

using namespace std;
using namespace CGLA;

bool Directional::sample(const Vec3f& pos, Vec3f& dir, Vec3f& L) const
{
  // Compute output and return value given the following information.
  //
  // Input:  pos (the position of the geometry in the scene)
  //
  // Output: dir (the direction toward the light)
  //         L   (the radiance received from the direction dir)
  //
  // Return: true if not in shadow
  //
  // Relevant data fields that are available (see Directional.h and Light.h):
  // shadows    (on/off flag for shadows)
  // tracer     (pointer to ray tracer)
  // light_dir  (direction of the emitted light)
  // emission   (radiance of the emitted light)
  //
  // Hint: Construct a shadow ray using the Ray datatype (it has a constructor 
  //       taking the initial position and direction of the ray as arguments)
  
  return false;
}

string Directional::describe() const
{
  ostringstream ostr;
  ostr << "Directional light (emitted radiance " << emission << ", direction " << light_dir << ").";
  return ostr.str();
}
