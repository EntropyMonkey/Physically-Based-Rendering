// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef LIGHT_H
#define LIGHT_H

#include <GL/glew.h>

class Light
{
public:
  Light() { }

  virtual void set_light(GLenum light) const = 0;
};

#endif // LIGHT_H
