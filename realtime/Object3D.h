// Written by Jeppe Revall Frisvad, 2011
// Copyright (c) DTU Informatics 2011

#ifndef OBJECT3D_H
#define OBJECT3D_H

#include "CGLA/Vec3f.h"
#include "CGLA/Mat4x4f.h"
#include "AABB.h"

struct Ray;

class Object3D
{
public:
  virtual bool intersect(Ray& r, unsigned int prim_idx) const = 0;
  virtual void transform(const CGLA::Mat4x4f& m) = 0;
  virtual AABB compute_bbox() const = 0;
  virtual void compute_bsphere(CGLA::Vec3f& center, float& radius) const
  {
    AABB bbox = compute_bbox();
    center = bbox.get_center();
    radius = length(bbox.get_diagonal())*0.5f;
  }
  virtual AABB get_primitive_bbox(unsigned int prim_idx) const { return compute_bbox(); }
  virtual unsigned int get_no_of_primitives() const { return 1; }
};

#endif // OBJECT3D_H