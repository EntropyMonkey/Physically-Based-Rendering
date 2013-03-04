// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2010
// Copyright (c) DTU Informatics 2010

#ifndef MCVOLUME_H
#define MCVOLUME_H

#include "CGLA/Vec3f.h"
#include "../optprops/Medium.h"
#include "Ray.h"
#include "PathTracer.h"
#include "Volume.h"

class MCVolume : public Volume
{
public:
  MCVolume(PathTracer* pathtracer, 
           unsigned int no_of_ray_samples = 1,
           unsigned int no_of_phase_samples = 1,
           int no_of_splits = 1) 
    : Volume(pathtracer, no_of_splits), 
      ray_samples(no_of_ray_samples), 
      phase_samples(no_of_phase_samples),
      use_multiple_scattering(true)
  { }

  virtual CGLA::Vec3f shade(Ray& r, bool emit = true) const;

protected:
  CGLA::Vec3f split_shade(const Ray& r, bool emit, const Medium* m) const;
  CGLA::Vec3f volume_shade(const Ray& r, bool emit, const Medium* m) const;

  unsigned int ray_samples;
  unsigned int phase_samples;
  bool use_multiple_scattering;
};

#endif // MCVOLUME_H
