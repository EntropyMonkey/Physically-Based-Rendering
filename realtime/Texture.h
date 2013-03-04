// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glut.h>
#include "CGLA/Vec4f.h"
#include "GLGraphics/SOIL.h"

class Texture
{
public:
  Texture() : width(0), height(0), data(0), fdata(0), clamp(false) { }
  ~Texture() { SOIL_free_image_data(data); delete [] fdata; }

  // Load texture from file
  void load(const char* filename);

  // Was a texture loaded yet
  bool has_texture() const { return data != 0; }

  // Look up the texel using texture space coordinates
  CGLA::Vec4f sample_nearest(float u, float v) const;
  CGLA::Vec4f sample_linear(float u, float v) const;

  // Clamp the texture
  void clamp_to_edge() { clamp = true; }

  // Repeat the texture
  void repeat() { clamp = false; }

  // OpenGL
  void bind() const { glBindTexture(GL_TEXTURE_2D, tex_handle); }
  void enable() const { glEnable(GL_TEXTURE_2D); }
  void disable() const { glDisable(GL_TEXTURE_2D); }

protected:
  CGLA::Vec4f look_up(unsigned int idx) const;
  float convert(unsigned char c) const;

  // Texture dimensions
  int width;
  int height;

  // Pointers to image data
  unsigned char* data;
  CGLA::Vec4f* fdata;

  // OpenGL texture handle
  GLuint tex_handle;

  // If clamp is false the texture is repeated
  bool clamp;

  // Bytes per pixel
  int channels;
};

#endif // TEXTURE_H
