// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef SPHERETEXTURE_H
#define SPHERETEXTURE_H

#include "CGLA/Vec3f.h"
#include "CGLA/Vec4f.h"
#include "HDRTexture.h"

class SphereTexture : public HDRTexture
{
public:
  CGLA::Vec4f sample_nearest(const CGLA::Vec3f& direction) const;
  CGLA::Vec4f sample_linear(const CGLA::Vec3f& direction) const;
  void project_direction(const CGLA::Vec3f& d, float& u, float& v) const;
};

#endif // SPHERETEXTURE_H
