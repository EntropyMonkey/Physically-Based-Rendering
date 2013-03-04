// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef SHLIGHT_H
#define SHLIGHT_H

#include <vector>
#include <GL/glew.h>
#include "CGLA/Vec3f.h"
#include "CGLA/Vec2d.h"

class SHLight
{
public:
  SHLight(unsigned int no_of_bands) 
    : bands(no_of_bands), coeffs(no_of_bands*no_of_bands, CGLA::Vec3f(0.0f)) 
  { }

  virtual void compute_sh() = 0;
  virtual void upload_sh(GLhandleARB program) const;

protected:
  CGLA::Vec2d vec_to_spherical(CGLA::Vec3f v);

  unsigned int bands;
  std::vector<CGLA::Vec3f> coeffs;
};


#endif // SHLIGHT_H
