// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2013
// Copyright (c) DTU Compute 2013

#ifndef RAYCASTER_H
#define RAYCASTER_H

#include <vector>
#include "CGLA/Vec2f.h"
#include "CGLA/Vec3f.h"
#include "SphereTexture.h"
#include "SunSky.h"
#include "Tracer.h"

class RayCaster : public Tracer
{
public:
  RayCaster(unsigned int w, unsigned int h, Scene* s, unsigned int pixel_subdivs = 1)
    : Tracer(w, h, s), subdivs(pixel_subdivs), background(1.0), sphere_tex(0), sun_sky(0)
  { 
    compute_jitters();
  }

  void set_background(const CGLA::Vec3f& color) { background = color; }
  void set_background(SphereTexture* sphere_texture) { sphere_tex = sphere_texture; }
  void set_background(PreethamSunSky* sun_and_sky) { sun_sky = sun_and_sky; }
  const CGLA::Vec3f& get_background() const { return background; }
  CGLA::Vec3f get_background(const CGLA::Vec3f& direction) const;
  void increment_pixel_subdivs();
  void decrement_pixel_subdivs();
  virtual CGLA::Vec3f compute_pixel(unsigned int x, unsigned int y) const;

protected:
  void compute_jitters();

  unsigned int subdivs;
  std::vector<CGLA::Vec2f> jitter;
  CGLA::Vec2f win_to_vp;
  CGLA::Vec2f lower_left;
  CGLA::Vec2f step;
  CGLA::Vec3f background;
  SphereTexture* sphere_tex;
  PreethamSunSky* sun_sky;
};

#endif // RAYCASTER_H
