// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef SHOMNI_H
#define SHOMNI_H

#include <GL/glew.h>
#include "CGLA/Vec3f.h"
#include "SphereTexture.h"
#include "SHLight.h"

class SHOmni : public SHLight
{
public:
  SHOmni(unsigned int no_of_bands, 
         unsigned int no_of_samples, 
         const CGLA::Vec3f& background_color, 
         const SphereTexture* texture = 0) 
    : SHLight(no_of_bands), samples(no_of_samples), background(background_color), tex(texture) 
  { }

  virtual void compute_sh();

protected:
  CGLA::Vec3f sample_sphere() const;

  unsigned int samples;
  CGLA::Vec3f background;
  const SphereTexture* tex;
};

#endif // SHOMNI_H
