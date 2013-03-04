// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2010
// Copyright (c) DTU Informatics 2010

#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <GL/glew.h>
#include "CGLA/Vec3f.h"
#include "TriMesh.h"
#include "Scene.h"

class CubeMap
{
public:
  CubeMap(unsigned int face_size = 128, GLenum tex_iformat = GL_RGB, GLenum tex_format = GL_RGB)
    : size(face_size), iformat(tex_iformat), format(tex_format), cube(0), fbo(0), depth_rb(0), center(0.0f)
  { }

  unsigned int get_handle() const { return cube; }

  void init();
  void init(unsigned int face_size, GLenum iformat, GLenum format);
  void erase();

  void enable() const;
  void disable() const;

  void generate(Scene* s, const TriMesh* object);
  void generate(void (*callback)(), const TriMesh* object);

  const CGLA::Vec3f& get_center() const { return center; } 

private:
  void before_draw(int face) const;
  void snapshot(Scene* s, int face) const;
  void snapshot(void (*callback)(), int face) const;

  unsigned int size;     // size of each cube face texture
  GLenum iformat;        // internal format of textures 
  GLenum format;         // format of textures
  unsigned int cube;     // cube map handle
  unsigned int fbo;      // framebuffer object handle
  unsigned int depth_rb; // renderbuffer handle for depth component
  CGLA::Vec3f center;    // position from where the cube map is captured
  float radius;          // distance to near clipping plane
};

#endif // CUBEMAP_H
