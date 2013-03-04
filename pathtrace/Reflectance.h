// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef REFLECTANCE_H
#define REFLECTANCE_H

#include "CGLA/Vec3f.h"
#include "Ray.h"
#include "ObjMaterial.h"
#include "Emission.h"

class Reflectance : public Emission
{
public:
  virtual CGLA::Vec3f shade(Ray& r, bool emit = true) const 
  { 
    return get_diffuse(r) + Emission::shade(r, emit); 
  }

protected:
  virtual CGLA::Vec3f get_diffuse(const Ray& r) const
  {
    const ObjMaterial* m = r.get_hit_material();
    return m ? CGLA::Vec3f(m->diffuse[0], m->diffuse[1], m->diffuse[2]) : CGLA::Vec3f(0.8f);
  }
};

#endif // REFLECTANCE_H
