// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2013
// Copyright (c) DTU Compute 2013

#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "CGLA/Vec3f.h"
#include "Ray.h"
#include "RayTracer.h"
#include "Light.h"

class PointLight : public Light
{
public:
  PointLight(RayTracer* ray_tracer, const CGLA::Vec3f& emitted_intensity, const CGLA::Vec3f& position) 
    : Light(ray_tracer), intensity(emitted_intensity), light_pos(position)
  { }

  virtual bool sample(const CGLA::Vec3f& pos, CGLA::Vec3f& dir, CGLA::Vec3f& L) const;
  virtual bool emit(Ray& r, CGLA::Vec3f& Phi) const;

protected:
  CGLA::Vec3f light_pos;
  CGLA::Vec3f intensity;
};

#endif // POINTLIGHT_H
