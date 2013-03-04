// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef REFLECTANCE_H
#define REFLECTANCE_H

#include "ObjMaterial.h"
#include "Emission.h"

class Reflectance : public Emission
{
public:
  virtual void init();
  virtual void set_material(const ObjMaterial* m);

protected:
  void set_diffuse(const ObjMaterial* m) const;
};

#endif // REFLECTANCE_H
