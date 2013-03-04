// Code from GEL (http://www.imm.dtu.dk/GEL/)
// Inspired by Nate Robins Obj loader.
// Modified by Jeppe Revall Frisvad to suit render framework.
// Copyright (c) DTU Informatics 2012

#include <iostream>
#include <cstdio>
#include <string>
#include "CGLA/Vec3f.h"
#include "Ray.h"
#include "IndexedFaceSet.h"
#include "Object3D.h"
#include "TriMesh.h"

using namespace std;
using namespace CGLA;

bool intersect_triangle(const Ray& ray, 
                        const Vec3f& v0, 
                        const Vec3f& v1, 
                        const Vec3f& v2, 
                        Vec3f& n,
                        float& t,
                        float& v,
                        float& w)
{
  // Find vectors for edges sharing v0
  Vec3f e0 = v1 - v0;
  Vec3f e1 = v0 - v2;

  // Find face normal
  n = cross( e0, e1 );

  // Compute ray-plane intersection
  float q = dot(ray.direction, n);
  if(fabsf(q) < 1.0e-12f)
    return false;
  q = 1.0f/q;
  Vec3f o_to_v0 = v0 - ray.origin;
  t = dot(o_to_v0, n)*q;

  // Check distance to intersection
  if(t < ray.tmin || t > ray.tmax)
    return false;

  // Find barycentric coordinates
  Vec3f n_tmp = cross(o_to_v0, ray.direction);
  v = dot(n_tmp, e1)*q;
  if(v < 0.0f) 
    return false;
  w = dot(n_tmp, e0)*q;
  if(w < 0.0f || v + w > 1.0f)
    return false;

  // Let the counterclockwise wound side face forward
  n = -n;
  return true;
}

bool TriMesh::intersect(Ray& r, unsigned int prim_idx) const
{
  Vec3i face = geometry.face(prim_idx);
  Vec3f n;
  float t, v, w;
  if(intersect_triangle(r, geometry.vertex(face[0]), geometry.vertex(face[1]), geometry.vertex(face[2]), n, t, v, w))
  {
    r.has_hit = true;
    r.dist = t;
    r.u = v;
    r.v = w;
    if(has_normals())
    {
      face = normals.face(prim_idx);
      r.hit_normal = normalize(normals.vertex(face[0])*(1.0f - v - w) + normals.vertex(face[1])*v + normals.vertex(face[2])*w);
    }
    else
      r.hit_normal = normalize(n);
    
    r.hit_object = this;
    r.hit_face_id = prim_idx;
    //if(material->has_texture && texcoords.no_faces() > 0)
    //{
    //  face = texcoords.face(prim_idx);
    //  hit.texcoord = u*texcoords.vertex(face.x) + v*texcoords.vertex(face.y) + w*texcoords.vertex(face.z);
    //}
    return true;
  }
  return false;
}

void TriMesh::transform(const Mat4x4f& m)
{
  for(unsigned int i = 0; i < geometry.no_vertices(); ++i)
    geometry.vertex_rw(i) = m.mul_3D_point(geometry.vertex(i));
  for(unsigned int i = 0; i < normals.no_vertices(); ++i)
    normals.vertex_rw(i) = m.mul_3D_vector(normals.vertex(i));
}

AABB TriMesh::get_primitive_bbox(unsigned int prim_idx) const
{
  AABB bbox;
  Vec3i face = geometry.face(prim_idx);
  bbox.add_point(geometry.vertex(face[0]));
  bbox.add_point(geometry.vertex(face[1]));
  bbox.add_point(geometry.vertex(face[2]));
  return bbox;
}

AABB TriMesh::compute_bbox() const
{
  AABB bbox;
  for(unsigned int i = 0; i < geometry.no_vertices(); ++i) 
    bbox.add_point(geometry.vertex(i));
  return bbox;
}

unsigned int TriMesh::find_material(const string& name) const
{
  for(unsigned int i = 0; i < materials.size(); ++i)
  {
    if(materials[i].name == name)
      return i;
  }
  return 0;
}

void TriMesh::compute_normals()
{		
  // By default the normal faces are the same as the geometry faces
  // and there are just as many normals as vertices, so we simply
  // copy.
  normals = geometry;

  // The normals are initialized to zero.
  for(unsigned int i = 0; i < normals.no_vertices(); ++i)
    normals.vertex_rw(i) = Vec3f(0.0f);

  // For each face
  for(unsigned int i = 0; i < geometry.no_faces(); ++i)
  {
    // Compute the normal
    const Vec3i& f  = geometry.face(i);
    const Vec3f& p0 = geometry.vertex(f[0]);
    const Vec3f& a  = geometry.vertex(f[1]) - p0;
    const Vec3f& b  = geometry.vertex(f[2]) - p0;
    Vec3f face_normal = cross(a, b);
    float len = dot(face_normal, face_normal);
    if(len > 0.0f)
    face_normal /= sqrt(len);

    // Add the angle weighted normal to each vertex
    for(int j = 0; j < 3; ++j)
    {
      const Vec3f& p0 = geometry.vertex(f[j]);
      Vec3f a = geometry.vertex(f[(j + 1)%3]) - p0;
      float len_a = dot(a, a);
      if(len_a > 0.0f)
        a /= sqrt(len_a);
      Vec3f b = geometry.vertex(f[(j + 2)%3]) - p0;
      float len_b = dot(b, b);
      if(len_b > 0.0f)
        b /= sqrt(len_b);
      float d = std::max(-1.0f, min(1.0f, dot(a,b)));
      normals.vertex_rw(f[j]) += face_normal*acos(d);
    }
  }

  // Normalize all normals
  for(unsigned int i = 0; i < normals.no_vertices(); ++i)
  {
    const Vec3f& normal = normals.vertex(i);
    float len_normal = dot(normal, normal);
    if(len_normal > 0.0f)
      normals.vertex_rw(i) /= sqrt(len_normal);
  }
}

void TriMesh::flip_normals()
{
  for(unsigned int i = 0; i < normals.no_vertices(); ++i)
  {
    Vec3f& n = normals.vertex_rw(i);
    n = -n;
  }
}

void TriMesh::compute_areas()
{
  int no_of_faces = geometry.no_faces();
  surface_area = 0.0f;
  face_areas.resize(no_of_faces);
  face_area_cdf.resize(no_of_faces);
	for(int i = 0; i < no_of_faces; ++i)
  {
		const Vec3i& f  = geometry.face(i);
		const Vec3f& p0 = geometry.vertex(f[0]);
		const Vec3f& a  = geometry.vertex(f[1]) - p0;
		const Vec3f& b  = geometry.vertex(f[2]) - p0;
    face_areas[i] = 0.5f*length(cross(a, b));
    face_area_cdf[i] = surface_area + face_areas[i];
    surface_area += face_areas[i];
  }
  if(surface_area > 0.0f)
    for(int i = 0; i < no_of_faces; ++i)
      face_area_cdf[i] /= surface_area;
}
