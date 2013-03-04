// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef HDRTEXTURE_H
#define HDRTEXTURE_H

#include "CGLA/Vec4f.h"
#include "Texture.h"

class HDRTexture : public Texture
{
public:
  void load_hdr(const char* filename);

protected:
  CGLA::Vec4f look_up(unsigned int idx);
  float convert(unsigned char c, int e);
};

#endif // HDRTEXTURE_H
