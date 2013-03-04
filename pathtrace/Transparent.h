// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef TRANSPARENT_H
#define TRANSPARENT_H

#include "CGLA/Vec3f.h"
#include "Ray.h"
#include "PathTracer.h"
#include "Mirror.h"

class Transparent : public Mirror
{
public:
  Transparent(PathTracer* pathtracer, int no_of_splits = 1, int max_trace_depth = 20) 
    : Mirror(pathtracer, max_trace_depth),
      splits(no_of_splits)
  { }

  virtual CGLA::Vec3f shade(Ray& r, bool emit = true) const;

protected:
  CGLA::Vec3f split_shade(Ray& r, bool emit) const;

  int splits;
};

#endif // TRANSPARENT_H
