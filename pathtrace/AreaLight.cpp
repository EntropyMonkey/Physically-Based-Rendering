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

  // this method uses monte carlo integration to create soft shadows

  // Get geometry info
  const IndexedFaceSet& geometry = mesh->geometry;
	const IndexedFaceSet& normals = mesh->normals;

  // get random triangle
  int triangleIndex = randomizer.mt_random_int32() % geometry.no_faces();
  
  // get index for vertices of triangle
  Vec3i vertexIndex = geometry.face(triangleIndex);
  // get index for normals of triangle
  Vec3i normalIndex = normals.face(triangleIndex);

  // get random position on triangle
  // ref: http://mathworld.wolfram.com/TrianglePointPicking.html
  float sqrt_e1 = sqrt(randomizer.mt_random());
  float e2 = randomizer.mt_random();

  // sample barycentric coordinates
  float u = 1 - sqrt_e1;
  float v = (1 - e2) * sqrt_e1;
  float w = e2 * sqrt_e1;
  
  // linear interpolation of vertices and normals, to get a point on the triangle and the according normal
  Vec3f lightPosition = Vec3f(0.0f);
  Vec3f lightNormal = Vec3f(0.0f);
  
  Vec3f uvw = Vec3f(u, v, w);
  for (int i=0; i<3; i++)
  {
    lightPosition += geometry.vertex(vertexIndex[i]) * uvw[i];
    lightNormal += normals.vertex(normalIndex[i]) * uvw[i];
  }
  lightNormal.normalize();
  
  // get light direction and distance to light
  Vec3f lightDirection = lightPosition - pos;
  float lightDistance = length(lightDirection);

  // set area light direction, normalize
  dir = lightDirection / lightDistance;
  
  // emission is scaled by geometry.no_faces() bec only 1/n are sampled
  Vec3f emission = mesh->face_areas[triangleIndex] * get_emission(triangleIndex) * geometry.no_faces();
  // set radiance
  L = emission * max(dot(lightNormal, -dir), 0.0f) / (lightDistance * lightDistance);

  // trace for shadows
  bool inShadow = false;
  if (shadows)
  {
    Ray shadowRay(pos, dir);
    shadowRay.tmax = lightDistance - 0.1111f;
    inShadow = tracer->trace(shadowRay);
  }

  return !inShadow;
}

bool AreaLight::simpleSample(const Vec3f& pos, Vec3f& dir, Vec3f& L) const

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
  
  // Get geometry info
  const IndexedFaceSet& geometry = mesh->geometry;
	const IndexedFaceSet& normals = mesh->normals;

  // averaged light position
  Vec3f lightPosition = Vec3f(0.0f);
  // averaged normals
  Vec3f lightNormal = Vec3f(0.0f);
  // emission summed up from all faces
  Vec3f emission = Vec3f(0.0f);

  // iterate over all faces
  for (int i = 0; i < geometry.no_faces(); i++)
  {
    // get the center of the face
    Vec3i face = geometry.face(i);
    Vec3f v0 = geometry.vertex(face[0]);
    Vec3f v1 = geometry.vertex(face[1]);
    Vec3f v2 = geometry.vertex(face[2]);
    Vec3f faceCenter = v0 + (v1 - v0 + v2 - v0) * 0.5f;
    
    // combine light position
    lightPosition += faceCenter;

    // average normals
    lightNormal += (normals.vertex(face[0]) + normals.vertex(face[1]) + normals.vertex(face[2])) / 3;

    // add emission
    emission += mesh->face_areas[i] * get_emission(i);
  }

  // average light position
  lightPosition /= geometry.no_faces();

  lightNormal.normalize();
  
  // get light direction and distance to light
  Vec3f lightDirection = lightPosition - pos;
  float lightDistance = length(lightDirection);

  // set area light direction, normalize
  dir = lightDirection / lightDistance;

  // set radiance
  L = emission * std::max(dot(-dir, lightNormal), 0.0f) / (lightDistance * lightDistance);

  // trace for shadows
  bool inShadow = false;
  if (shadows)
  {
    Ray shadowRay(pos, dir);
    shadowRay.tmax = lightDistance - 0.1111f;
    inShadow = tracer->trace(shadowRay);
  }

  return !inShadow;
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
