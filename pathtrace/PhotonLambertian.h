// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef PHOTONLAMBERTIAN_H
#define PHOTONLAMBERTIAN_H

#include "CGLA/Vec3f.h"
#include "Ray.h"
#include "ParticleTracer.h"
#include "PhotonCaustics.h"

class PhotonLambertian : public PhotonCaustics
{
public:
  PhotonLambertian(ParticleTracer* particle_tracer, 
                   const std::vector<Light*>& light_vector, 
                   float max_distance_in_estimate,
                   int no_of_photons_in_estimate,
                   bool use_final_gathering = true,
                   unsigned int no_of_samples = 1,
                   const PhotonCaustics* caustics_shader = 0) 
    : PhotonCaustics(particle_tracer, light_vector, max_distance_in_estimate, no_of_photons_in_estimate),
      gather(use_final_gathering),
      samples(no_of_samples),
      caustics(caustics_shader)
  { }

  virtual CGLA::Vec3f shade(Ray& r, bool emit = true) const;

  bool is_gathering() const { return gather; }
  void toggle_final_gathering() { gather = !gather; }

protected:
  CGLA::Vec3f split_shade(Ray& r, bool emit) const;
  CGLA::Vec3f shade_new_ray(Ray& r) const;

  bool gather;
  unsigned int samples;
  const PhotonCaustics* caustics;
};

#endif // PHOTONLAMBERTIAN_H