// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef VOLUME_H
#define VOLUME_H

#include <climits>
#include "CGLA/Vec3f.h"
#include "../optprops/Medium.h"
#include "Ray.h"
#include "PathTracer.h"
#include "Transparent.h"

class Volume : public Transparent
{
public:
  Volume(PathTracer* pathtracer, int no_of_splits = 1) : Transparent(pathtracer, no_of_splits, 500) { }

  virtual CGLA::Vec3f shade(Ray& r, bool emit = true) const;

protected:
  CGLA::Vec3f split_shade(Ray& r, bool emit) const;

  virtual CGLA::Vec3f get_transmittance(Ray& r, const Medium* m) const;
  double get_luminance(const CGLA::Vec3f& color) const;

  static const double scene_scale;
};

#endif // VOLUME_H

