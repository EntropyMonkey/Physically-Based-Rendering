// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef PATHTRACER_H
#define PATHTRACER_H

#include "CGLA/Vec3f.h"
#include "Ray.h"
#include "RayTracer.h"

class PathTracer : public RayTracer
{
public:
  PathTracer(unsigned int w, unsigned int h, Scene* s, unsigned int pixel_subdivs = 1)
    : RayTracer(w, h, s, pixel_subdivs)
  { }  

	void update_pixel(unsigned int x, unsigned int y, float sample_number, CGLA::Vec3f& L) const;

  bool trace_cosine_weighted(const Ray& in, Ray& out) const;
  bool trace_hemisphere(const Ray& in, Ray& out) const;
  bool trace_isotropic(const Ray& in, Ray& out, double distance) const;
  bool trace_HG(const Ray& in, Ray& out, double distance, double g) const;
  bool trace_Phong_distribution(const Ray& in, Ray& out) const;
  bool trace_Blinn_distribution(const Ray& in, Ray& out) const;
};

#endif
