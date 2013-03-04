// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2013
// Copyright (c) DTU Compute 2013

#include <cmath>
#include "CGLA/Vec3f.h"
#include "CGLA/Vec3i.h"
#include "../optprops/Medium.h"
#include "IndexedFaceSet.h"
#include "ObjMaterial.h"
#include "mt_random.h"
#include "sampler.h"
#include "AreaLight.h"

using namespace std;
using namespace CGLA;

bool AreaLight::sample(const Vec3f& pos, Vec3f& dir, Vec3f& L) const

{
  // Get geometry info
  const IndexedFaceSet& geometry = mesh->geometry;
	const IndexedFaceSet& normals = mesh->normals;
	const float no_of_faces = geometry.no_faces();

  // Compute output and return value given the following information.
  //
  // Input:  pos (the position of the geometry in the scene)
  //
  // Output: dir (the direction toward the light)
  //         L   (the radiance received from the direction dir)
  //
  // Return: true if not in shadow
  //
  // Relevant data fields that are available (see Light.h and above):
  // shadows      (on/off flag for shadows)
  // tracer       (pointer to ray tracer)
  // geometry     (indexed face set of triangle vertices)
  // normals      (indexed face set of vertex normals)
  // no_of_faces  (number of faces in triangle mesh for light source)
  //
  // Hint: (a) Use the dist field of a ray to limit the distance it
  //       is to be traced.
  //       (b) The framework includes a function normalize(v) which 
  //       returns the vector v normalized to length 1.
  
  return false;  
}

bool AreaLight::emit(Ray& r, Vec3f& Phi) const
{
  // Generate and trace a ray carrying radiance emitted from this area light.
  //
  // Output: r   (the new ray)
  //         Phi (the flux carried by the emitted ray)
  //
  // Return: true if the ray hits anything when traced
  //
  // Relevant data fields that are available (see Light.h and Ray.h):
  // tracer       (pointer to ray tracer)
  // geometry     (indexed face set of triangle vertices)
  // normals      (indexed face set of vertex normals)
  // no_of_faces  (number of faces in triangle mesh for light source)
  // r.origin     (starting position of ray)
  // r.direction  (direction of ray)
  //
  // Hint: Use the function sample_cosine_weighted(...) to sample a
  //       direction for the new ray.

  // Get geometry info
  const IndexedFaceSet& geometry = mesh->geometry;
	const IndexedFaceSet& normals = mesh->normals;
	const float no_of_faces = geometry.no_faces();

  // Sample ray origin and direction
 
  // Trace ray
  
  // If a surface was hit, compute Phi and return true

  return false;
}

Vec3f AreaLight::get_emission(size_t triangle_id) const
{
  const ObjMaterial& mat = mesh->materials[mesh->mat_idx[triangle_id]];
  const Medium* med = tracer->get_medium(mat.name);
  if(med->emissive)
  {
    const Color<double>& c_emission = med->get_emission(rgb);
    return Vec3f(c_emission[0], c_emission[1], c_emission[2]);
  }
  return Vec3f(0.0f);
}
