// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include "TriMesh.h"
#include "ObjMaterial.h"

class Shader
{
public:
  void init(const char* vert_shader, const char* frag_shader = 0);
  void init(const char* path, const char* vert_file, const char* frag_file);
  virtual void init() = 0;
  virtual void set_material(const ObjMaterial* m) { }
  virtual void set_attributes(const TriMesh* mesh, int index) { }
  virtual void enable();
	virtual void disable();

protected:
	GLuint program; // shader program
};

#endif // SHADER_H
