// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#include "CGLA/Vec4f.h"
#include "CGLA/Vec3f.h"
#include "TriMesh.h"
#include "IndexedFaceSet.h"
#include "mt_random.h"
#include "AABB.h"
#include "AreaLight.h"

using namespace CGLA;

AreaLight::AreaLight(const TriMesh* triangle_mesh)
  : mesh(triangle_mesh), center(0.0f), direction(0.0f), emission(0.0f), area(0.0)
{ 
  // Compute center and general direction
	const IndexedFaceSet& geometry = mesh->geometry;
	const IndexedFaceSet& normals = mesh->normals;
  AABB bbox;
  Vec3f L_e(0.0f);
  for(unsigned int i = 0; i < geometry.no_faces(); ++i)
  {
	  const Vec3i& face = geometry.face(i);
	  const Vec3f& v0 = geometry.vertex(face[0]);
	  const Vec3f& v1 = geometry.vertex(face[1]);
	  const Vec3f& v2 = geometry.vertex(face[2]);
    area += 0.5f*cross(v1 - v0, v2 - v0).length();
	  bbox.add_point(v0);
	  bbox.add_point(v1);
	  bbox.add_point(v2);

    const Vec3i& nface = normals.face(i);
	  direction += normals.vertex(nface[0]);
	  direction += normals.vertex(nface[1]);
	  direction += normals.vertex(nface[2]);

    const float* ambient = mesh->materials[mesh->mat_idx[i]].ambient;
    L_e += Vec3f(ambient[0], ambient[1], ambient[2]);
  }
  emission = Vec4f(L_e*(area/static_cast<double>(geometry.no_faces())), 1.0f);
  center = Vec4f(bbox.get_center(), 1.0f);
  direction = normalize(direction);
}

void AreaLight::set_light(GLenum light) const
{
  // Set the properties of the GL light given as argument using the
  // data fields of this area light class.
  //
  // Input:  light (GL light identifier)
  //
  // Output: Calls to glLightfv
  //
  // Relevant data fields that are available (see AreaLight.h):
  // emission   (radiance of the emitted light)
  // center     (center of the area light)
  // direction  (average direction of the area light)
  //
  // Hint: The vector datatypes in the framework have a member function
  //       get() such that light_dir.get(), for example, retrieves a
  //       pointer to the data in the vector.
}

void AreaLight::sample(Vec3f& position, Vec3f& normal) const
{
  // Get geometry info
  const IndexedFaceSet& geometry = mesh->geometry;
	const IndexedFaceSet& normals = mesh->normals;
	const float no_of_faces = geometry.no_faces();

  // Sample a triangle face

  // Sample Barycentric coordinates

  // Compute position

  // Compute normal
}