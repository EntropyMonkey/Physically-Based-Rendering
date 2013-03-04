#ifndef MT_RANDOM_H
#define MT_RANDOM_H

#include "Randomizer.h"
extern Randomizer randomizer;

inline double mt_random()
{
  return randomizer.mt_random();
}

// Use the following function in constructors that 
// could be used for global variable instances.
inline double safe_mt_random()
{
  return randomizer.safe_mt_random();
}

#endif // MT_RANDOM_H
