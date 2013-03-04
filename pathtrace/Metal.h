// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef METAL_H
#define METAL_H

#include <climits>
#include "CGLA/Vec3f.h"
#include "Ray.h"
#include "PathTracer.h"
#include "Mirror.h"

class Metal : public Mirror
{
public:
  Metal(PathTracer* pathtracer, int no_of_splits = 1) 
    : Mirror(pathtracer, INT_MAX), splits(no_of_splits)
  { }

  virtual CGLA::Vec3f shade(Ray& r, bool emit = true) const;

protected:
  int splits;
};

#endif // METAL_H
