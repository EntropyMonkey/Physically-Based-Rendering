// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef GLOSSY_H
#define GLOSSY_H

#include <vector>
#include "CGLA/Vec3f.h"
#include "Ray.h"
#include "ObjMaterial.h"
#include "Light.h"
#include "Lambertian.h"

class Glossy : public Lambertian
{
public:
  Glossy(const std::vector<Light*>& light_vector) : Lambertian(light_vector) { }

  virtual CGLA::Vec3f shade(Ray& r, bool emit = true) const;

protected:
  CGLA::Vec3f get_specular(const Ray& r) const
  {
    const ObjMaterial* m = r.get_hit_material();
    return m ? CGLA::Vec3f(m->specular[0], m->specular[1], m->specular[2]) : CGLA::Vec3f(0.0f);
  }

  float get_shininess(const Ray& r) const
  {
    const ObjMaterial* m = r.get_hit_material();
    return m ? m->shininess : 0.0f;
  }
};

#endif // GLOSSY_H
