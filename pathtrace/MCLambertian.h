// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef MCLAMBERTIAN_H
#define MCLAMBERTIAN_H

#include <vector>
#include "CGLA/Vec3f.h"
#include "Ray.h"
#include "PathTracer.h"
#include "Light.h"
#include "Glossy.h"

class MCLambertian : public Glossy
{
public:
  MCLambertian(PathTracer* path_tracer, 
               const std::vector<Light*>& light_vector, 
               unsigned int no_of_samples = 1) 
    : Glossy(light_vector), tracer(path_tracer), samples(no_of_samples)
  { }

  virtual CGLA::Vec3f shade(Ray& r, bool emit = true) const;

protected:
  CGLA::Vec3f split_shade(Ray& r, bool emit) const;
  CGLA::Vec3f shade_new_ray(Ray& r) const;
  double get_luminance(const CGLA::Vec3f& color) const;

  PathTracer* tracer;
  unsigned int samples;
};

#endif // MCLAMBERTIAN_H
