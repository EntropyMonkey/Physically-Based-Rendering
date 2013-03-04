// Code from GEL (http://www.imm.dtu.dk/GEL/)
// Modified by Jeppe Revall Frisvad to suit render framework.
// Copyright (c) DTU Informatics 2012

#ifndef RAY_H
#define RAY_H

#include "CGLA/Vec3f.h"
#include "CGLA/Vec3i.h"
#include "TriMesh.h"
#include "ObjMaterial.h"

/// Represents a ray as used for ray tracing.
struct Ray 
{
  // Constructor
  Ray() 
    : has_hit(false), inside(false), did_hit_diffuse(false),
      ior(1.0f), tmin(1.0e-4f), tmax(CGLA::BIG), trace_depth(0), hit_object(0)
  { }

  Ray(const CGLA::Vec3f& _origin, const CGLA::Vec3f& _direction, float min_dist = 1.0e-4f, float max_dist = CGLA::BIG) 
    : origin(_origin), direction(_direction),
      has_hit(false), inside(false), did_hit_diffuse(false),
      ior(1.0f), tmin(min_dist), tmax(max_dist), trace_depth(0), hit_object(0)
  { }

  CGLA::Vec3f origin;
  CGLA::Vec3f direction;
  CGLA::Vec3f hit_pos;
  CGLA::Vec3f hit_normal;

  bool has_hit;          // Did the ray hit an object
  bool inside;           // Is the ray inside an object
  bool did_hit_diffuse;  // Did the path involve a diffuse object

  float dist;            // Distance from origin to current intersection
  float ior;             // Current index of refraction for media
  float u, v;            // uv-coordinates on current surface
  float tmin, tmax;      // Near and far cut-off planes

  int trace_depth;       // Current recursion depth (path length)
  unsigned int hit_face_id;

  const TriMesh* hit_object;

  const ObjMaterial* get_hit_material() const
  {
    return hit_object ? &hit_object->materials[hit_object->mat_idx[hit_face_id]] : 0;
  }

  void reset()
  {
    has_hit = false;
    inside = false;
    did_hit_diffuse = false;
    ior = 1.0f;
    tmin = 0.0f;
    tmax = CGLA::BIG;
    trace_depth = 0;
    hit_object = 0;
  }
};

#endif // RAY_H
