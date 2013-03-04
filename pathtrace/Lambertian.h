// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef LAMBERTIAN_H
#define LAMBERTIAN_H

#include <vector>
#include "CGLA/Vec3f.h"
#include "Ray.h"
#include "Light.h"
#include "Textured.h"

class Lambertian : public Textured
{
public:
  Lambertian(const std::vector<Light*>& light_vector) : lights(light_vector) { }

  virtual CGLA::Vec3f shade(Ray& r, bool emit = true) const;

protected:
  const std::vector<Light*>& lights;
};

#endif // LAMBERTIAN_H
