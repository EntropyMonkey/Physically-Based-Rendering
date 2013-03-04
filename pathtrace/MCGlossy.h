// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef MCGLOSSY_H
#define MCGLOSSY_H

#include <vector>
#include "CGLA/Vec3f.h"
#include "Ray.h"
#include "Light.h"
#include "MCLambertian.h"

class MCGlossy : public MCLambertian
{
public:
  MCGlossy(PathTracer* path_tracer, 
           const std::vector<Light*>& light_vector, 
           unsigned int no_of_samples = 1) 
    : MCLambertian(path_tracer, light_vector, no_of_samples)
  { }

  virtual CGLA::Vec3f shade(Ray& r, bool emit = true) const;

protected:
  CGLA::Vec3f split_shade(Ray& r, bool emit) const;
};

#endif // MCGLOSSY_H
