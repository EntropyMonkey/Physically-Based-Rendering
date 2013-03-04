// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef SHADER_H
#define SHADER_H

#include "CGLA/Vec3f.h"
#include "Ray.h"

class Shader
{
public:
  virtual CGLA::Vec3f shade(Ray& r, bool emit = true) const = 0;
};

#endif // SHADER_H
