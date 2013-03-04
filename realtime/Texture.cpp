// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#include <iostream>
#include <GL/glut.h>
#include "CGLA/Vec4f.h"
#include "GLGraphics/SOIL.h"
#include "Texture.h"

using namespace std;
using namespace CGLA;

void Texture::load(const char* filename)
{
  data = SOIL_load_image(filename, &width, &height, &channels, SOIL_LOAD_AUTO);
  if(!data)
  {
    cerr << "Error: Could not load texture image file." << endl;
    return;
  }
  int img_size = width*height;
  fdata = new Vec4f[img_size];
  for(int i = 0; i < img_size; ++i)
    fdata[i] = look_up(i);
  tex_handle = SOIL_create_OGL_texture(data, width, height, channels, tex_handle, SOIL_FLAG_INVERT_Y);
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
    u = u - static_cast<int>(u);
    if(u < 0.0) ++u;
    v = v - static_cast<int>(v);
    if(v < 0.0) ++v;
  }

  unsigned int U = static_cast<unsigned int>(u*width);
  unsigned int V = static_cast<unsigned int>(v*height);
  if(U == width) --U;
  if(V == 0) ++V;
  unsigned int idx = U + (height - V)*width;

  return fdata[idx];
}

Vec4f Texture::sample_linear(float u, float v) const
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
    u = u - static_cast<int>(u);
    if(u < 0.0) ++u;
    v = v - static_cast<int>(v);
    if(v < 0.0) ++v;
  }

  u *= width;
  v *= height;
  unsigned int U = static_cast<int>(u - 0.5f);
  unsigned int V = static_cast<int>(v - 0.5f);
  int idx = U + V*width;
  if(U == width - 1) 
    return fdata[idx];
  u -= 0.5f + U;
  v -= 0.5f + V;

  Vec4f a = fdata[idx]*(1.0f - u) + fdata[idx + 1]*u;
  if(V == height - 1)
    return a;
  idx += height;
  Vec4f b = fdata[idx]*(1.0f - u) + fdata[idx + 1]*u;

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
