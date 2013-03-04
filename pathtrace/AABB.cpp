#include <cstdlib>
#include "GL/glut.h"

#include "CGLA/Vec3f.h"
#include "CGLA/Mat4x4f.h"
#include "AABB.h"

using namespace CGLA;

CGLA::Vec3f (AABB::*get_corner[8])() const 
  = { &AABB::get_lower_left_far,
      &AABB::get_lower_left_near,
      &AABB::get_lower_right_far,
      &AABB::get_lower_right_near,
      &AABB::get_upper_left_far,
      &AABB::get_upper_left_near,
      &AABB::get_upper_right_far,
      &AABB::get_upper_right_near  };

void (AABB::*set_gl_ortho[6])() const
  = { &AABB::set_gl_ortho_left,
      &AABB::set_gl_ortho_right,
      &AABB::set_gl_ortho_bottom,
      &AABB::set_gl_ortho_top,
      &AABB::set_gl_ortho_near,
      &AABB::set_gl_ortho_far      };

void AABB::set_gl_ortho_left() const 
{ 
  Vec3f center = get_center();

  Mat4x4f rot_mat = rotation_Mat4x4f(YAXIS, -static_cast<float>(M_PI_2));

  Vec4f rot_min = rot_mat*Vec4f(p_min, 1.0f);
  Vec4f rot_max = rot_mat*Vec4f(p_max, 1.0f);
  AABB rot_bbox;

  rot_bbox.add_point(rot_min[0], rot_min[1], rot_min[2]);
  rot_bbox.add_point(rot_max[0], rot_max[1], rot_max[2]);

  glMatrixMode(GL_PROJECTION); 
  glLoadIdentity();
  glOrtho(rot_bbox.p_min[0], rot_bbox.p_max[0], 
	        rot_bbox.p_min[1], rot_bbox.p_max[1], 
	        rot_bbox.p_min[2], rot_bbox.p_max[2]); 

  glMatrixMode(GL_MODELVIEW);
  gluLookAt(0.0, 0.0, 0.0,
	          1.0, 0.0, 0.0,
	          0.0, 1.0, 0.0);
}

void AABB::set_gl_ortho_right() const 
{ 
  Vec3f center = get_center();

  Mat4x4f rot_mat = rotation_Mat4x4f(YAXIS, static_cast<float>(M_PI_2));

  Vec4f rot_min = rot_mat*Vec4f(p_min, 1.0f);
  Vec4f rot_max = rot_mat*Vec4f(p_max, 1.0f);
  AABB rot_bbox;

  rot_bbox.add_point(rot_min[0], rot_min[1], rot_min[2]);
  rot_bbox.add_point(rot_max[0], rot_max[1], rot_max[2]);

  glMatrixMode(GL_PROJECTION); 
  glLoadIdentity();
  glOrtho(rot_bbox.p_min[0], rot_bbox.p_max[0], 
	        rot_bbox.p_min[1], rot_bbox.p_max[1], 
	        rot_bbox.p_min[2], rot_bbox.p_max[2]); 

  glMatrixMode(GL_MODELVIEW);
  gluLookAt(0.0, 0.0, 0.0,
	          -1.0, 0.0, 0.0,
	          0.0, 1.0, 0.0);
}

void AABB::set_gl_ortho_bottom() const 
{ 
  Vec3f center = get_center();

  Mat4x4f rot_mat = rotation_Mat4x4f(XAXIS, -static_cast<float>(M_PI_2));

  Vec4f rot_min = rot_mat*Vec4f(p_min, 1.0f);
  Vec4f rot_max = rot_mat*Vec4f(p_max, 1.0f);
  AABB rot_bbox;

  rot_bbox.add_point(rot_min[0], rot_min[1], rot_min[2]);
  rot_bbox.add_point(rot_max[0], rot_max[1], rot_max[2]);

  glMatrixMode(GL_PROJECTION); 
  glLoadIdentity();
  glOrtho(rot_bbox.p_min[0], rot_bbox.p_max[0], 
	        rot_bbox.p_min[1], rot_bbox.p_max[1], 
	        rot_bbox.p_min[2], rot_bbox.p_max[2]); 

  glMatrixMode(GL_MODELVIEW);
  gluLookAt(0.0, 0.0, 0.0,
	          0.0, 1.0, 0.0,
	          0.0, 0.0, 1.0);
}

void AABB::set_gl_ortho_top() const 
{ 
  Vec3f center = get_center();

  Mat4x4f rot_mat = rotation_Mat4x4f(XAXIS, static_cast<float>(M_PI_2));

  Vec4f rot_min = rot_mat*Vec4f(p_min, 1.0f);
  Vec4f rot_max = rot_mat*Vec4f(p_max, 1.0f);
  AABB rot_bbox;

  rot_bbox.add_point(rot_min[0], rot_min[1], rot_min[2]);
  rot_bbox.add_point(rot_max[0], rot_max[1], rot_max[2]);

  glMatrixMode(GL_PROJECTION); 
  glLoadIdentity();
  glOrtho(rot_bbox.p_min[0], rot_bbox.p_max[0], 
          rot_bbox.p_min[1], rot_bbox.p_max[1], 
          rot_bbox.p_min[2], rot_bbox.p_max[2]); 

  glMatrixMode(GL_MODELVIEW);
  gluLookAt(0.0, 0.0, 0.0,
	          0.0, -1.0, 0.0,
	          0.0, 0.0, 1.0);
}

void AABB::set_gl_ortho_near() const 
{ 
  Vec3f center = get_center();

  Mat4x4f rot_mat = rotation_Mat4x4f(YAXIS, static_cast<float>(M_PI));

  Vec4f rot_min = rot_mat*Vec4f(p_min, 1.0f);
  Vec4f rot_max = rot_mat*Vec4f(p_max, 1.0f);
  AABB rot_bbox;

  rot_bbox.add_point(rot_min[0], rot_min[1], rot_min[2]);
  rot_bbox.add_point(rot_max[0], rot_max[1], rot_max[2]);

  glMatrixMode(GL_PROJECTION); 
  glLoadIdentity();
  glOrtho(rot_bbox.p_min[0], rot_bbox.p_max[0], 
	        rot_bbox.p_min[1], rot_bbox.p_max[1], 
	        rot_bbox.p_min[2], rot_bbox.p_max[2]); 

  glMatrixMode(GL_MODELVIEW);
  gluLookAt(0.0, 0.0, 0.0,
	          0.0, 0.0, 1.0,
	          0.0, 1.0, 0.0);
}

void AABB::set_gl_ortho_far() const 
{ 
  glMatrixMode(GL_PROJECTION); 
  glLoadIdentity();
  glOrtho(p_min[0], p_max[0], p_min[1], p_max[1], p_min[2], p_max[2]); 
  glMatrixMode(GL_MODELVIEW);
}
