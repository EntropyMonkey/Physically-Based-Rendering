// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2010
// Copyright (c) DTU Informatics 2010

#ifndef SOFTSHADOW_H
#define SOFTSHADOW_H

#include "Scene.h"
#include "Shadow.h"

class SoftShadow : public Shadow
{
public:
  SoftShadow(Scene* the_scene, unsigned int no_of_samples) 
    : Shadow(the_scene), samples(no_of_samples) 
  { }

  virtual void init();
  virtual void enable();
	virtual void disable();

protected:
  unsigned int samples;
  unsigned int sample_map[2];
};

#endif // SOFTSHADOW_H
