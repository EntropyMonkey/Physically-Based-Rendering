// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef SHDIRECTIONAL_H
#define SHDIRECTIONAL_H

#include "CGLA/Vec3f.h"
#include "Directional.h"
#include "SHLight.h"

class SHDirectional : public SHLight
{
public:
  SHDirectional(unsigned int no_of_bands, const Directional& dir)
    : SHLight(no_of_bands), emission(dir.emission), light_dir(dir.light_dir)
  { }

  virtual void compute_sh();

protected:
  CGLA::Vec3f emission;
  CGLA::Vec3f light_dir;
};

#endif // SHDIRECTIONAL_H
