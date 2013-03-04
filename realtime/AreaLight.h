// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef AREALIGHT_H
#define AREALIGHT_H

#include <GL/glew.h>
#include "CGLA/Vec4f.h"
#include "CGLA/Vec3f.h"
#include "TriMesh.h"
#include "Light.h"

class AreaLight : public Light
{
public:
  AreaLight(const TriMesh* triangle_mesh); 

  virtual void set_light(GLenum light) const;

  const TriMesh* get_mesh() const { return mesh; }

  void sample(CGLA::Vec3f& position, CGLA::Vec3f& normal) const;

protected:
  const TriMesh* mesh;
  CGLA::Vec4f emission;
  CGLA::Vec4f center;
  CGLA::Vec3f direction;
  double area;
};

#endif // AREALIGHT_H
