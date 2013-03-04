// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef RANDOMIZER_H
#define RANDOMIZER_H

#include <valarray>

class Randomizer
{
public:
  Randomizer(unsigned long seed = 5489UL);

  /* generates a random number on [0,0xffffffff]-interval */
  unsigned long mt_random_int32();
  
  /* generates a random number on [0,1]-real-interval */
  double mt_random();

  /* generates a random number on [0,1)-real-interval */
  double mt_random_half_open();

  /* generates a random number on (0,1)-real-interval */
  double mt_random_open();

private:
  static const int N;
  static const int M;

  /* the array for the state vector  */
  std::valarray<unsigned int> mt; 
  int mti;                        
};

#endif
