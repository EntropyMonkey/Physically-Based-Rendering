// Written by Jeppe Revall Frisvad, 2011
// Copyright (c) DTU Informatics 2011

#ifndef ACCELERATOR_H
#define ACCELERATOR_H

#include <vector>
#include "Ray.h"
#include "AccObj.h"
#include "Object3D.h"
#include "Plane.h"

class Accelerator
{
public:
  virtual ~Accelerator();
  virtual void init(const std::vector<const TriMesh*>& geometry, const std::vector<const Plane*>& scene_planes);
  virtual bool closest_hit(Ray& r) const;
  virtual bool any_hit(Ray& r) const;

protected:
  void closest_plane(Ray& r) const;
  bool any_plane(Ray& r) const;

  std::vector<AccObj*> primitives;
  std::vector<const Plane*> planes;
};

#endif // ACCELERATOR_H