// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef EMISSION_H
#define EMISSION_H

#include "CGLA/Vec3f.h"
#include "Ray.h"
#include "ObjMaterial.h"
#include "Shader.h"

class Emission : virtual public Shader
{
public:
  virtual CGLA::Vec3f shade(Ray& r, bool emit = true) const 
  { 
    return emit ? get_emission(r) : CGLA::Vec3f(0.0f); 
  }

protected:
  CGLA::Vec3f get_emission(const Ray& r) const
  {
    const ObjMaterial* m = r.get_hit_material();
    return m ? CGLA::Vec3f(m->ambient[0], m->ambient[1], m->ambient[2]) : CGLA::Vec3f(0.2f);
  }
};

#endif // EMISSION_H
