// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef GLOSSY_H
#define GLOSSY_H

#include <vector>
#include "ObjMaterial.h"
#include "Light.h"
#include "Lambertian.h"

class Glossy : public Lambertian
{
public:
  Glossy(const std::vector<const Light*>& light_vector) : Lambertian(light_vector) { }

  virtual void init();
  virtual void set_material(const ObjMaterial* m);

protected:
  void set_specular(const ObjMaterial* m) const;
  void set_shininess(const ObjMaterial* m) const;
};

#endif // GLOSSY_H
