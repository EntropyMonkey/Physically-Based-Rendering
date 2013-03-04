#ifndef AABB_H
#define AABB_H

#include "CGLA/CGLA.h"
#include "CGLA/Vec3f.h"

struct AABB
{
  CGLA::Vec3f p_min, p_max;

  AABB() : p_min(CGLA::BIG), p_max(-CGLA::BIG) { }
  AABB(const CGLA::Vec3f& min_point, const CGLA::Vec3f& max_point) : p_min(min_point), p_max(max_point) { }

  void reset()
  {
    p_max.set(-CGLA::BIG, -CGLA::BIG, -CGLA::BIG);
    p_min.set(CGLA::BIG, CGLA::BIG, CGLA::BIG);
  }

  void add_point(const CGLA::Vec3f& p)
  {
    p_min = v_min(p_min, p);
    p_max = v_max(p_max, p);
  }

  void add_point(float x, float y, float z)
  {
    add_point(CGLA::Vec3f(x, y, z));
  }

  void add_AABB(const AABB& other)
  {
    p_min = v_min(p_min, other.p_min);
    p_max = v_max(p_max, other.p_max);
  }

  bool intersects(const AABB& other) const
  {
    for(unsigned int i = 0; i < 3; ++i)
      if(other.p_min[i] > p_max[i] || other.p_max[i] < p_min[i])
        return false;
    return true;
  }

  float area() const
  {
    const CGLA::Vec3f d = get_diagonal();
    return 2.0f*(d[0]*d[1] + d[1]*d[2] + d[2]*d[0]);
  }

  CGLA::Vec3f get_center() const { return (p_max + p_min)*0.5f; }
  CGLA::Vec3f get_diagonal() const { return p_max - p_min; }

  // The following descriptive function names refer to a the case where
  // you look down the negative z-axis in a right handed coordinate system
  CGLA::Vec3f get_lower_left_far() const { return p_min; }
  CGLA::Vec3f get_lower_left_near() const { return CGLA::Vec3f(p_min[0], p_min[1], p_max[2]); }
  CGLA::Vec3f get_lower_right_far() const { return CGLA::Vec3f(p_max[0], p_min[1], p_min[2]); }
  CGLA::Vec3f get_lower_right_near() const { return CGLA::Vec3f(p_max[0], p_min[1], p_max[2]); }
  CGLA::Vec3f get_upper_left_far() const { return CGLA::Vec3f(p_min[0], p_max[1], p_min[2]); }
  CGLA::Vec3f get_upper_left_near() const { return CGLA::Vec3f(p_min[0], p_max[1], p_max[2]); }
  CGLA::Vec3f get_upper_right_far() const { return CGLA::Vec3f(p_max[0], p_max[1], p_min[2]); }
  CGLA::Vec3f get_upper_right_near() const { return p_max; }

  // Functions to set orthographic view along an axis through the center of
  // the bounding box
  void set_gl_ortho_left() const;
  void set_gl_ortho_right() const;
  void set_gl_ortho_bottom() const; 
  void set_gl_ortho_top() const;
  void set_gl_ortho_near() const;
  void set_gl_ortho_far() const;
};

// The numbering of the corners follows from the convention described above
extern CGLA::Vec3f (AABB::*get_corner[8])() const;

// The numbering of the faces follows: left, right, bottom, top, near, far
extern void (AABB::*set_gl_ortho[6])() const;

#endif
