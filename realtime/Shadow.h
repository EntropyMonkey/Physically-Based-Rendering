// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2010
// Copyright (c) DTU Informatics 2010

#ifndef SHADOW_H
#define SHADOW_H

#include <GL/glew.h>
#include "AreaLight.h"
#include "Scene.h"
#include "CubeMap.h"
#include "Lambertian.h"

class Shadow : public Lambertian
{
public:
  Shadow(Scene* the_scene) 
    : Lambertian(the_scene->get_lights()), scene(the_scene), light(0), light_idx(0),
      cube_map(512, GL_LUMINANCE16F_ARB, GL_LUMINANCE)
  { }

  void init(const char* vert_shader, const char* frag_shader);

  virtual void init();
  virtual void enable();
	virtual void disable();

  void set_light_idx(unsigned int idx) { light_idx = idx; }
  void generate_map();

protected:
  const AreaLight* light;
  Scene* scene;
  CubeMap cube_map;
  unsigned int light_idx;
};

#endif // SHADOW_H
