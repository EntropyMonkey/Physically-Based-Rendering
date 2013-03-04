// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef GAMMA_H
#define GAMMA_H

#include "ToneMap.h"

class Gamma : public ToneMap
{
public:
  Gamma(double gamma) : exponent(1.0/gamma) { }

  virtual void apply(float* data, unsigned int width, unsigned int height, unsigned int channels) const;
  virtual void unapply(float* data, unsigned int width, unsigned int height, unsigned int channels) const;

protected:
  double gamma_correct(double value) const;
  double gamma_uncorrect(double value) const;

  double exponent;
};

#endif // GAMMA_H
