// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef EMISSION_H
#define EMISSION_H

#include "ObjMaterial.h"
#include "Shader.h"

class Emission : public Shader
{
public:
  virtual void init();
  virtual void set_material(const ObjMaterial* m);

protected:
  void set_ambient(const ObjMaterial* m) const;
};

#endif // EMISSION_H
