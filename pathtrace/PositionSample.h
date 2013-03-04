#ifndef POSITIONSAMPLE_H
#define POSITIONSAMPLE_H

#include "CGLA/Vec3f.h"

struct PositionSample
{
  PositionSample() : initialized(false) { }  

  bool initialized;
  CGLA::Vec3f pos;
  CGLA::Vec3f normal;
  CGLA::Vec3f L;
  size_t hit_face_id;
  float u, v;
};

#endif // POSITIONSAMPLE_H