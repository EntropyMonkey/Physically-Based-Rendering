// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef MT_RANDOM_H
#define MT_RANDOM_H

#include "Randomizer.h"
extern Randomizer randomizer;

inline double mt_random()
{
  return randomizer.mt_random();
}

#endif // MT_RANDOM_H
