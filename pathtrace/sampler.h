// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2012
// Copyright (c) DTU Informatics 2012

#ifndef SAMPLER_H
#define SAMPLER_H

#include <cmath>
#include "CGLA/Vec3f.h"
#include "mt_random.h"

// Given a direction vector v sampled on the hemisphere
// over a surface point with the z-axis as its normal,
// this function applies the same rotation to v as is
// needed to rotate the z-axis to the actual normal
// [Frisvad, Journal of Graphics Tools 16, 2012].
inline void rotate_to_normal(const CGLA::Vec3f& normal, CGLA::Vec3f& v)
{
	if(normal[2] < -0.9999999f)
  {
	  v = CGLA::Vec3f(-v[1], -v[0], -v[2]);
    return;
  }
  const float a = 1.0f/(1.0f + normal[2]);
  const float b = -normal[0]*normal[1]*a;
  v =   CGLA::Vec3f(1.0f - normal[0]*normal[0]*a, b, -normal[0])*v[0] 
      + CGLA::Vec3f(b, 1.0f - normal[1]*normal[1]*a, -normal[1])*v[1] 
      + normal*v[2];
}

// Given spherical coordinates, where theta is the 
// polar angle and phi is the azimuthal angle, this
// function returns the corresponding direction vector
inline CGLA::Vec3f spherical_direction(double sin_theta, double cos_theta, double phi)
{
  return CGLA::Vec3f(sin_theta*cos(phi), sin_theta*sin(phi), cos_theta);
}

inline CGLA::Vec3f sample_hemisphere(const CGLA::Vec3f& normal)
{
  // Get random numbers

	// Calculate new direction as if the z-axis were the normal

  // Rotate from z-axis to actual normal and return
  return CGLA::Vec3f(0.0f);
}

inline CGLA::Vec3f sample_cosine_weighted(const CGLA::Vec3f& normal)
{
  // ref: http://www.rorydriscoll.com/2009/01/07/better-sampling/
  // ref: http://pathtracing.wordpress.com/2011/03/03/cosine-weighted-hemisphere/
  // Get random numbers
  const float r1 = randomizer.mt_random();
  const float r2 = randomizer.mt_random();

  const float theta = acos(sqrt(1.0f - r1));
  const float phi = 2.0f * M_PI * r2;

	// Calculate new direction as if the z-axis were the normal
  CGLA::Vec3f _normal(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));

  // Rotate from z-axis to actual normal and return
  rotate_to_normal(normal, _normal);
  return _normal;
}

inline CGLA::Vec3f sample_Phong_distribution(const CGLA::Vec3f& normal, const CGLA::Vec3f& dir, float shininess)
{
  // Get random numbers

	// Calculate sampled direction as if the z-axis were the reflected direction

  // Rotate from z-axis to actual reflected direction
  return CGLA::Vec3f(0.0f);
}

inline CGLA::Vec3f sample_Blinn_distribution(const CGLA::Vec3f& normal, const CGLA::Vec3f& dir, float shininess)
{
  // Get random numbers

	// Calculate sampled half-angle vector as if the z-axis were the normal

  // Rotate from z-axis to actual normal

  // Make sure that the half-angle vector points in the right direction

  // Return the reflection of "dir" around the half-angle vector
  return CGLA::Vec3f(0.0f);
}

inline CGLA::Vec3f sample_isotropic()
{
  // Use rejection sampling to find an arbitrary direction
  return CGLA::Vec3f(0.0f);
}

inline CGLA::Vec3f sample_HG(const CGLA::Vec3f& forward, double g)
{
  // Get random numbers

	// Calculate new direction as if the z-axis were the forward direction

  // Rotate from z-axis to forward direction
  return CGLA::Vec3f(0.0f);
}

#endif
