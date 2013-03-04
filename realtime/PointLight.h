// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include <GL/glew.h>
#include "CGLA/Vec3f.h"
#include "CGLA/Vec4f.h"
#include "Light.h"

class PointLight : public Light
{
public:
  PointLight(const CGLA::Vec3f& emitted_intensity, const CGLA::Vec3f& position)
    : intensity(emitted_intensity, 1.0f), light_pos(position, 1.0f)
  { }

  virtual void set_light(GLenum light) const;

protected:
  CGLA::Vec4f intensity;
  CGLA::Vec4f light_pos;
};

#endif // POINTLIGHT_H
