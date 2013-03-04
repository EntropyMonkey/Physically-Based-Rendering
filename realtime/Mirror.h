// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef MIRROR_H
#define MIRROR_H

#include <map>
#include "TriMesh.h"
#include "Scene.h"
#include "CubeMap.h"
#include "Shader.h"

class Mirror : public Shader
{
public:
  Mirror(Scene* the_scene) : scene(the_scene), current_map(0) { }

  void init(const char* vert_shader, const char* frag_shader);
  virtual void init();
  virtual void enable();
	virtual void disable();

  void generate_map(const TriMesh* mesh);
  void set_current_map(const TriMesh* mesh) { current_map = &cube_maps[mesh]; }

protected:
  Scene* scene;
  std::map<const TriMesh*, CubeMap> cube_maps;
  CubeMap* current_map;
};

#endif // MIRROR_H
