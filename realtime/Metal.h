// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef METAL_H
#define METAL_H

#include "ObjMaterial.h"
#include "Scene.h"
#include "Mirror.h"

class Metal : public Mirror
{
public:
  Metal(Scene* the_scene) : Mirror(the_scene) { }

  virtual void init();
  virtual void set_material(const ObjMaterial* m);
};

#endif // METAL_H
