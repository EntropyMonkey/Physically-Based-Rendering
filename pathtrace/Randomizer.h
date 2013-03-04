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

  // Randomizer.cpp defines a global instance, this potentially
  // leads to problems if used with constructors for other global 
  // variables. The safe_mt_random function checks if initalization
  // is needed before computing the random number.
  double safe_mt_random() { if(mt.size() == 0) init(); return mt_random(); }

private:
  void init(unsigned long seed = 5489UL);

  static const int N;
  static const int M;

  /* the array for the state vector  */
  std::valarray<unsigned int> mt; 
  int mti;                        
};

#endif
