// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef MIRROR_H
#define MIRROR_H

#include "CGLA/Vec3f.h"
#include "Ray.h"
#include "PathTracer.h"
#include "Shader.h"

class Mirror : virtual public Shader
{
public:
  Mirror(PathTracer* pathtracer, int max_trace_depth = 20) 
    : tracer(pathtracer), max_depth(max_trace_depth)
  { }

  virtual CGLA::Vec3f shade(Ray& r, bool emit = true) const;

protected:
  CGLA::Vec3f shade_new_ray(Ray& r) const;
  
  PathTracer* tracer;
  int max_depth;
};

#endif // MIRROR_H
