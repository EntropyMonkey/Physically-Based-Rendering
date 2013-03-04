// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef LAMBERTIAN_H
#define LAMBERTIAN_H

#include <vector>
#include "Light.h"
#include "Reflectance.h"

class Lambertian : public Reflectance
{
public:
  Lambertian(const std::vector<const Light*>& light_vector) : lights(light_vector) { }

  virtual void init();
  virtual void enable();
	virtual void disable();

protected:
  const std::vector<const Light*>& lights;
};

#endif // LAMBERTIAN_H
