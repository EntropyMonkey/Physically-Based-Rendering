// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef LIGHT_H
#define LIGHT_H

#include "CGLA/Vec3f.h"
#include "Ray.h"

class RayTracer;

class Light
{
public:
  Light(RayTracer* ray_tracer, unsigned int no_of_samples = 1) 
    : tracer(ray_tracer), samples(no_of_samples), shadows(true) 
  { }

  virtual bool sample(const CGLA::Vec3f& pos, CGLA::Vec3f& dir, CGLA::Vec3f& L) const = 0;
  virtual bool emit(Ray& r, CGLA::Vec3f& Phi) const { return false; }
  virtual bool intersect(const Ray& r, CGLA::Vec3f& L) const { return false; }

  unsigned int get_no_of_samples() const { return samples; }

  void toggle_shadows() { shadows = !shadows; }
  bool generating_shadows() const { return shadows; }

protected:
  bool shadows;
  unsigned int samples;
  RayTracer* tracer;
};

#endif // LIGHT_H
