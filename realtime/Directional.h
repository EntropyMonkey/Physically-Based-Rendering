// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef DIRECTIONAL_H
#define DIRECTIONAL_H

#include <GL/glew.h>
#include "CGLA/Vec3f.h"
#include "CGLA/Vec4f.h"
#include "Light.h"

class Directional : public Light
{
public:
  Directional(const CGLA::Vec3f& emitted_radiance, const CGLA::Vec3f& light_direction) 
    : emission(emitted_radiance, 1.0f), light_dir(light_direction, 0.0f) 
  { }

  virtual void set_light(GLenum light) const;
  void set_direction(const CGLA::Vec3f& dir) { light_dir = CGLA::Vec4f(dir, 0.0f); }

protected:
  friend class SHDirectional;
  CGLA::Vec4f emission;
  CGLA::Vec4f light_dir;
};

#endif // DIRECTIONAL_H
