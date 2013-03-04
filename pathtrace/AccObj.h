// Written by Jeppe Revall Frisvad, 2011
// Copyright (c) DTU Informatics 2011

#ifndef ACCOBJ_H
#define ACCOBJ_H

#include "AABB.h"
#include "Object3D.h"

struct AccObj
{
  AccObj() : geometry(0), prim_idx(0) { }
  AccObj(const Object3D* object, unsigned int primitive_idx) 
    : geometry(object), prim_idx(primitive_idx), bbox(object->get_primitive_bbox(primitive_idx))
  { }

  const Object3D* geometry;
  unsigned int prim_idx;
  AABB bbox;
};

#endif // ACCOBJ_H
