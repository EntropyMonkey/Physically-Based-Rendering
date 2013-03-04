#ifndef PLANE_H
#define PLANE_H

#include "CGLA/Vec3f.h"
#include "CGLA/Vec3i.h"
#include "CGLA/Mat4x4f.h"
#include "Ray.h"
#include "TriMesh.h"
#include "ObjMaterial.h"
#include "Object3D.h"

class Plane : public Object3D
{
public:
  Plane(const CGLA::Vec3f& position, const CGLA::Vec3f& normal, const ObjMaterial& material, float tex_scale = 1.0f)
    : texcoord_scale(1.0f/tex_scale)
  { 
    CGLA::Vec3f n = normalize(normal);
    CGLA::onb(n, tangent, binormal);
    d = CGLA::dot(normal, -position);
    mesh.geometry.add_vertex(position);
    mesh.geometry.add_face(CGLA::Vec3i(0, 0, 0));
    mesh.normals.add_vertex(n);
    mesh.normals.add_face(CGLA::Vec3i(0, 0, 0));
    mesh.texcoords.add_vertex(CGLA::Vec3f(0.0f));
    mesh.texcoords.add_vertex(CGLA::Vec3f(1.0f, 0.0f, 0.0f));
    mesh.texcoords.add_vertex(CGLA::Vec3f(0.0f, 1.0f, 0.0f));
    mesh.texcoords.add_face(CGLA::Vec3i(0, 1, 2));
    mesh.materials.push_back(material);
    mesh.mat_idx.push_back(0);
  }

  virtual bool intersect(Ray& r, unsigned int prim_idx) const
  {
    r.hit_normal = mesh.normals.vertex(0);
    float cos_theta = CGLA::dot(r.hit_normal, r.direction);
    if(fabsf(cos_theta) < 1.0e-12f)
      return false;
    float t = -(dot(r.hit_normal, r.origin) + d)/cos_theta;
    if(t < r.tmin || t > r.tmax)
      return false;
    r.has_hit = true;
    r.dist = t;
    r.hit_object = &mesh;
    r.hit_face_id = prim_idx;
    if(mesh.materials[0].has_texture)
    {
      r.hit_pos = r.origin + r.direction*t;
      get_uv(r.hit_pos, r.u, r.v);
    }
    return true;
  }

  virtual void Plane::transform(const CGLA::Mat4x4f& m)
  {
    mesh.transform(m);
    CGLA::onb(mesh.normals.vertex(0), tangent, binormal);
    d = -CGLA::dot(mesh.geometry.vertex(0), mesh.normals.vertex(0));
  }

  virtual AABB Plane::compute_bbox() const 
  { 
    return AABB(CGLA::Vec3f(-CGLA::BIG), CGLA::Vec3f(CGLA::BIG)); 
  }

  const TriMesh* get_mesh() const { return &mesh; }
  const CGLA::Vec3f& get_origin() const { return mesh.geometry.vertex(0); }
  const CGLA::Vec3f& get_normal() const { return mesh.normals.vertex(0); }
  const CGLA::Vec3f& get_tangent() const { return tangent; }
  const CGLA::Vec3f& get_binormal() const { return binormal; }

  void get_uv(const CGLA::Vec3f& hit_pos, float& u, float& v) const 
  { 
    CGLA::Vec3f uv_displacement = hit_pos - mesh.geometry.vertex(0);
    u = texcoord_scale*dot(uv_displacement, tangent);
    v = texcoord_scale*dot(uv_displacement, binormal);
  }

protected:
  TriMesh mesh;
  CGLA::Vec3f tangent;
  CGLA::Vec3f binormal;
  float d;
  float texcoord_scale;
};

#endif // PLANE_H
