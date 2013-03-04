// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef TIMER_H
#define TIMER_H

#include <ctime>

class Timer
{
 public:
  Timer() : t1(0), t2(0) { }
  
  void start(double from_time = 0.0)
  {
    t1 = static_cast<int>(std::clock() - from_time*static_cast<double>(CLOCKS_PER_SEC));
  }

  double split()
  {
    return (std::clock() - t1) / static_cast<double>(CLOCKS_PER_SEC);
  }

  void stop()
  {
    t2 = std::clock();
  }
  
  double get_time()
  {
    return (t2 - t1) / static_cast<double>(CLOCKS_PER_SEC);
  }

 private:
  int t1;
  int t2;
};

class FrameRateTimer : public Timer
{
 public:
  FrameRateTimer(int avg = 40) : average(avg), fps(0.0) { }

  void set_average(int avg) 
  {
    average = avg;
  }

  double get_fps()
  {
    one_frame();
    return fps;
  }

 private:
  int average;
  double fps;

  void one_frame()
  {
    static int i = 0;

    if(i >= average)
    {
      stop();
      fps = average / get_time();
      i = 0;
    }
    if(i == 0)
      start();

    i++; 
  }
};

#endif
