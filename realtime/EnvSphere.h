// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef ENVSPHERE_H
#define ENVSPHERE_H

#include "Texture.h"
#include "Camera.h"
#include "Shader.h"

class EnvSphere : public Shader
{
public:
  EnvSphere(const Texture* texture) : tex(texture) { }

  virtual void init();
  virtual void enable();
	virtual void disable();

  void draw(const Camera& cam);

protected:
  const Texture* tex;
};

#endif // ENVSPHERE_H
