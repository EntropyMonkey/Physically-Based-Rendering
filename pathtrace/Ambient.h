// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef AMBIENT_H
#define AMBIENT_H

#include "CGLA/Vec3f.h"
#include "Ray.h"
#include "PathTracer.h"
#include "Lambertian.h"

class Ambient : public Lambertian
{
public:
  Ambient(PathTracer* path_tracer, const std::vector<Light*>& light_vector, unsigned int no_of_samples) 
    : Lambertian(light_vector), tracer(path_tracer), samples(no_of_samples)
  { }

  virtual CGLA::Vec3f shade(Ray& r, bool emit = true) const;

protected:
  PathTracer* tracer;
  unsigned int samples;
};

#endif // AMBIENT_H
