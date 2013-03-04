// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2010
// Copyright (c) DTU Informatics 2010

#include <iostream>
#include <GL/glut.h>
#include "CGLA/Vec4f.h"
#include "GLGraphics/SOIL.h"
#include "Texture.h"

using namespace std;
using namespace CGLA;

void Texture::load(const char* filename)
{
  SOIL_free_image_data(data);
  data = SOIL_load_image(filename, &width, &height, &channels, SOIL_LOAD_AUTO);
  if(!data)
  {
    cerr << "Error: Could not load texture image file." << endl;
    return;
  }
  int img_size = width*height;
  delete [] fdata;
  fdata = new Vec4f[img_size];
  for(int i = 0; i < img_size; ++i)
    fdata[i] = look_up(i);
  tex_handle = SOIL_create_OGL_texture(data, width, height, channels, tex_handle, SOIL_FLAG_INVERT_Y);
  tex_target = GL_TEXTURE_2D;
}

void Texture::load(GLenum target, GLuint texture)
{
  glBindTexture(target, texture);    
  glGetTexLevelParameteriv(target, 0, GL_TEXTURE_WIDTH, &width);
  glGetTexLevelParameteriv(target, 0, GL_TEXTURE_HEIGHT, &height);
  delete [] fdata;
  fdata = new Vec4f[width*height];
  glGetTexImage(target, 0, GL_RGBA, GL_FLOAT, fdata[0].get());
  tex_handle = texture;
  tex_target = target;
}

Vec4f Texture::sample_nearest(float u, float v) const
{
  if(!fdata)
    return Vec4f(0.0f);

  if(clamp)
  {
    u = std::min(std::max(u, 0.0f), 1.0f);
    v = std::min(std::max(v, 0.0f), 1.0f);
  }
  else
  {
    u = u - floorf(u);
    v = v - floorf(v);
  }

  int U = static_cast<int>(u*width + 0.5f);
  int V = static_cast<int>(v*height + 0.5f);
  if(U == width) U = clamp ? U - 1 : 0;
  if(V == height) V = clamp ? V - 1 : 0;
  unsigned int idx = U + (height - V - 1)*width;

  return fdata[idx];
}

Vec4f Texture::sample_linear(float u, float v) const
{
  if(!fdata)
    return Vec4f(0.0f);

  // Find uv-coordinates after clamp or repeat
  if(clamp)
  {
    u = std::min(std::max(u, 0.0f), 0.99999999f);
    v = std::min(std::max(v, 0.0f), 0.99999999f);
  }
  else
  {
    u = u - floorf(u);
    v = v - floorf(v);
  }

  // Map uv-coordinates to texel indices
  u *= width;
  v *= height;
  int U = static_cast<int>(u);
  int V = static_cast<int>(v);
  u -= U;
  v -= V;
  unsigned int idx = U + (height - V - 1)*width;
  unsigned int next_idx = idx + 1;
  if(U == width - 1)
    next_idx = clamp ? idx : next_idx - width;

  // Interpolate linearly between texels in the horizontal direction
  Vec4f a = fdata[idx]*(1.0f - u) + fdata[next_idx]*u;

  // Find texel indices in the row below the current one
  if(idx < static_cast<unsigned int>(width))
  {
    if(clamp) 
      return a;
    idx = U + (height - 1)*width; 
  }
  else
    idx -= width;
  next_idx = idx + 1;
  if(U == width - 1)
    next_idx = clamp ? idx : next_idx - width;

  // Complete bilinear interpolation to find final texture value
  Vec4f b = fdata[idx]*(1.0f - u) + fdata[next_idx]*u;
  return a*(1.0f - v) + b*v;
}

Vec4f Texture::look_up(unsigned int idx) const
{
  idx *= channels;
  switch(channels)
  {
  case 1: 
  {
    float v = convert(data[idx]);
    return Vec4f(v, v, v, 1.0f);
  }
  case 2: 
    return Vec4f(convert(data[idx]), convert(data[idx]), convert(data[idx]), convert(data[idx + 1]));
  case 3: 
    return Vec4f(convert(data[idx]), convert(data[idx + 1]), convert(data[idx + 2]), 1.0f);
  case 4: 
    return Vec4f(convert(data[idx]), convert(data[idx + 1]), convert(data[idx + 2]), convert(data[idx + 3]));
  }
  return Vec4f(0.0f);
}

float Texture::convert(unsigned char c) const
{
  return (c + 0.5f)/256.0f;
}
