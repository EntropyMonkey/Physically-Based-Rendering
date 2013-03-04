// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef DIRECTIONAL_H
#define DIRECTIONAL_H

#include <string>
#include "CGLA/Vec3f.h"
#include "RayTracer.h"
#include "Light.h"

class Directional : public Light
{
public:
  Directional(RayTracer* ray_tracer, const CGLA::Vec3f& emitted_radiance, const CGLA::Vec3f& light_direction) 
    : Light(ray_tracer), emission(emitted_radiance), light_dir(light_direction) 
  { }

  virtual bool sample(const CGLA::Vec3f& pos, CGLA::Vec3f& dir, CGLA::Vec3f& L) const;

  std::string describe() const;

protected:
  CGLA::Vec3f light_dir;
  CGLA::Vec3f emission;
};

#endif // DIRECTIONAL_H
