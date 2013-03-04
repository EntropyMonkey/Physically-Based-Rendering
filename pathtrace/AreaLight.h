// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2010
// Copyright (c) DTU Informatics 2010

#ifndef AREALIGHT_H
#define AREALIGHT_H

#include "CGLA/Vec3f.h"
#include "TriMesh.h"
#include "Ray.h"
#include "RayTracer.h"
#include "Light.h"

class AreaLight : public Light
{
public:
  AreaLight(RayTracer* ray_tracer, const TriMesh* triangle_mesh, unsigned int no_of_samples = 1) 
    : Light(ray_tracer, no_of_samples), mesh(triangle_mesh)
  { }

  virtual bool sample(const CGLA::Vec3f& pos, CGLA::Vec3f& dir, CGLA::Vec3f& L) const;
  virtual bool emit(Ray& r, CGLA::Vec3f& Phi) const;

protected:
  CGLA::Vec3f get_emission(size_t triangle_id) const;

  const TriMesh* mesh;
};

#endif // AREALIGHT_H
