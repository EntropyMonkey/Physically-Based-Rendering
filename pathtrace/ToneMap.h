// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef TONEMAP_H
#define TONEMAP_H

class ToneMap
{
public:
  virtual void apply(float* data, unsigned int width, unsigned int height, unsigned int channels) const = 0;
  virtual void unapply(float* data, unsigned int width, unsigned int height, unsigned int channels) const = 0;
};

#endif // TONEMAP_H
