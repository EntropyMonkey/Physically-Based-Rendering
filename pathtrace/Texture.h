// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2010
// Copyright (c) DTU Informatics 2010

#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glut.h>
#include "CGLA/Vec4f.h"
#include "GLGraphics/SOIL.h"

class Texture
{
public:
  Texture() : width(0), height(0), data(0), fdata(0), clamp(false), tex_handle(0), tex_target(GL_TEXTURE_2D) { }
  ~Texture() { SOIL_free_image_data(data); delete [] fdata; }

  // Load texture from file
  void load(const char* filename);

  // Load texture from OpenGL texture
  void load(GLenum target, GLuint texture);

  // Clear texture data
  void clear() { SOIL_free_image_data(data); data = 0; delete [] fdata; fdata = 0; }

  // Was a texture loaded yet
  bool has_texture() const { return fdata != 0; }

  // Look up the texel using texture space coordinates
  CGLA::Vec4f sample_nearest(float u, float v) const;
  CGLA::Vec4f sample_linear(float u, float v) const;

  // Clamp the texture
  void clamp_to_edge() { clamp = true; }

  // Repeat the texture
  void repeat() { clamp = false; }

  // OpenGL
  void bind() const { glBindTexture(tex_target, tex_handle); }
  void enable() const { glEnable(tex_target); }
  void disable() const { glDisable(tex_target); }

protected:
  CGLA::Vec4f look_up(unsigned int idx) const;
  float convert(unsigned char c) const;

  // Texture dimensions
  int width;
  int height;

  // Pointers to image data
  unsigned char* data;
  CGLA::Vec4f* fdata;

  // OpenGL texture info
  GLuint tex_handle;
  GLenum tex_target;

  // If clamp is false the texture is repeated
  bool clamp;

  // Bytes per pixel
  int channels;
};

#endif // TEXTURE_H
