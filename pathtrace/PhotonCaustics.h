// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef PHOTONCAUSTICS_H
#define PHOTONCAUSTICS_H

#include "CGLA/Vec3f.h"
#include "Ray.h"
#include "ParticleTracer.h"
#include "Glossy.h"

class PhotonCaustics : public Glossy
{
public:
  PhotonCaustics(ParticleTracer* particle_tracer, 
                 const std::vector<Light*>& light_vector, 
                 float max_distance_in_estimate,
                 int no_of_photons_in_estimate) 
    : Glossy(light_vector), 
      tracer(particle_tracer), 
      max_dist(max_distance_in_estimate), 
      photons(no_of_photons_in_estimate)
  { }

  virtual CGLA::Vec3f shade(Ray& r, bool emit = true) const;

protected:
  ParticleTracer* tracer;
  float max_dist;
  int photons;
};

#endif // PHOTONCAUSTICS_H