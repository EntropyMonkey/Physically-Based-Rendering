// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2010
// Copyright (c) DTU Informatics 2010

#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "CGLA/Vec3f.h"
#include "../optprops/Medium.h"
#include "Ray.h"
#include "Scene.h"
#include "RayCaster.h"

class RayTracer : public RayCaster
{
public:
  RayTracer(unsigned int w, unsigned int h, Scene* s, unsigned int pixel_subdivs = 1)
    : RayCaster(w, h, s, pixel_subdivs)
  { }

  bool trace(Ray& r) const { return scene->intersect(r); }
  bool trace_reflected(const Ray& in, Ray& out) const;
  bool trace_reflected(const Ray& in, Ray& out, double& fresnel_R) const;
  bool trace_reflected(const Ray& in, Ray& out, CGLA::Vec3f& fresnel_R) const;
  bool trace_refracted(const Ray& in, Ray& out) const;
  bool trace_refracted(const Ray& in, Ray& out, double& fresnel_R) const;
  bool trace_dispersive(const Ray& in, Ray& out, double& fresnel_R, 
                        unsigned int color_band, ColorType color_type = rgb) const;

private:
  double get_ior_out(const Ray& in, CGLA::Vec3f& dir, CGLA::Vec3f& normal, double& cos_theta_in) const;
  double get_ior_out(const Ray& in, CGLA::Vec3f& dir, CGLA::Vec3f& normal, double& cos_theta_in, 
                     unsigned int color_band, ColorType color_type) const;
};

#endif // RAYTRACER_H
