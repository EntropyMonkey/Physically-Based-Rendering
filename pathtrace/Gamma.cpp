// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#include <cmath>
#include "Gamma.h"

using namespace std;

void Gamma::apply(float* data, unsigned int w, unsigned int h, unsigned int channels) const
{
  for(unsigned int i = 0; i < w*h*channels; ++i)
    data[i] = static_cast<float>(gamma_correct(data[i]));
}

void Gamma::unapply(float* data, unsigned int w, unsigned int h, unsigned int channels) const
{
  for(unsigned int i = 0; i < w*h*channels; ++i)
    data[i] = static_cast<float>(gamma_uncorrect(data[i]));
}

double Gamma::gamma_correct(double value) const
{
  return pow(value, exponent);
}

double Gamma::gamma_uncorrect(double value) const
{
  return pow(value, 1.0/exponent);
}
