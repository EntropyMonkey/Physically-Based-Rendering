// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2013
// Copyright (c) DTU Compute 2013

#include "CGLA/Vec3f.h"
#include "CGLA/Vec3i.h"
#include "Ray.h"
#include "ObjMaterial.h"
#include "TriMesh.h"
#include "IndexedFaceSet.h"
#include "Texture.h"
#include "Textured.h"

using namespace CGLA;

Vec3f Textured::get_diffuse(const Ray& r) const
{
  if(!texs)
    return Reflectance::get_diffuse(r);

  const ObjMaterial* m = r.get_hit_material();
  if(m)
  {
    const Texture* tex = m->has_texture ? (*texs)[m->tex_name] : 0;
    if(tex && tex->has_texture())
    {
      const TriMesh* mesh = r.hit_object;
    	const IndexedFaceSet& texcoords = mesh->texcoords;
      const Vec3i& face = texcoords.face(r.hit_face_id);
      const Vec3f& t0 = texcoords.vertex(face[0]);
      const Vec3f& t1 = texcoords.vertex(face[1]);
      const Vec3f& t2 = texcoords.vertex(face[2]);
      Vec3f coords = t0*(1.0f - r.u - r.v) + t1*r.u + t2*r.v;
      return Vec3f(tex->sample_linear(coords[0], coords[1]));
    }
    return Vec3f(m->diffuse[0], m->diffuse[1], m->diffuse[2]);
  }
  return Vec3f(0.8f);
}
