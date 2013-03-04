// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2013
// Copyright (c) DTU Compute 2013

#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include "GL/glut.h"
#include "CGLA/Vec2f.h"
#include "CGLA/Vec3f.h"
#include "Ray.h"

const float NEAR_PLANE = 1.0e-3f;
const float FAR_PLANE = 1.0e5f;

#ifndef M_1_PI
#define M_1_PI 0.31830988618379067154
#endif

class Camera
{
public:
  Camera(const CGLA::Vec3f& eye_point = CGLA::Vec3f(0.0f),
         const CGLA::Vec3f& lookat_point = CGLA::Vec3f(0.0f, 0.0f, -1.0f),
         const CGLA::Vec3f& up_vector = CGLA::Vec3f(0.0f, 1.0f, 0.0f),
         float focal_distance = 1.0f)
  {
    set(eye_point, lookat_point, up_vector, focal_distance);
  }

  void set(const CGLA::Vec3f& eye_point,
           const CGLA::Vec3f& lookat_point,
           const CGLA::Vec3f& up_vector,
           float focal_distance)
  {
    eye = eye_point;
    lookat = lookat_point;
    up = up_vector;
    focal_dist = focal_distance;

    CGLA::Vec3f line_of_sight = lookat - eye;

    // Calculate view plane normal and basis
    ip_normal = CGLA::normalize(line_of_sight);
    if(1.0f - CGLA::dot(ip_normal, normalize(up)) < 1.0e-6f)
    {
      std::cerr << "Line of sight " << line_of_sight 
                << " and the camera up direction " << up
                << " are the same." << std::endl;
      exit(0);
    }
    ip_axes[0] = CGLA::normalize(CGLA::cross(ip_normal, up));
    ip_axes[1] = CGLA::normalize(CGLA::cross(ip_axes[0], ip_normal));

    // Account for focal distance
    ip_normal *= focal_dist;
  
    // Calculate field of view (using the pinhole camera model)
    float tmp = std::atan(0.5f/focal_dist);
    fov = 360.0*M_1_PI*tmp;
  }

  /// Get direction of viewing ray from image coords.
  CGLA::Vec3f get_ray_dir(const CGLA::Vec2f& coords) const
  {
    // vp_normal is multiplied by focal_dist in the constructor
    return ip_normal + ip_axes[0]*coords[0] + ip_axes[1]*coords[1];
  }

  /// Return position of camera.
  const CGLA::Vec3f& get_position() const { return eye; }

  /// Return camera line of sight
  const CGLA::Vec3f& get_line_of_sight() const { return ip_normal; }  

  /// Return the ray corresponding to a set of image coords
  Ray get_ray(const CGLA::Vec2f& coords) const
  {
    return Ray(eye, normalize(get_ray_dir(coords))); 
  }

  float get_fov() const { return fov; }
  float get_focal_dist() const { return focal_dist; }
  void set_focal_dist(float focal_distance) { set(eye, lookat, up, focal_distance); }

  // OpenGL

  void glSetPerspective(float width, float height) const
  {
    GLdouble aspect = width/height;    

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, aspect, focal_dist*NEAR_PLANE, FAR_PLANE);

    glMatrixMode(GL_MODELVIEW);
  }

  void glSetCamera() const
  {
    gluLookAt(eye[0], eye[1], eye[2], 
              lookat[0], lookat[1], lookat[2], 
              up[0], up[1], up[2]);
  }

private:

  CGLA::Vec3f eye, lookat, up;
  float focal_dist;
  float fov;

  // Basis of the camera coordinate system (ip - image plane)
  // The normal is the viewing direction.
  CGLA::Vec3f ip_normal;
  CGLA::Vec3f ip_axes[2]; 
};

#endif
