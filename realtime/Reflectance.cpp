// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#include <GL/glew.h>
#include "ObjMaterial.h"
#include "Reflectance.h"

namespace 
{
  GLchar vert_prog_str[] = 
  "void main()                                              \n"
  "{                                                        \n"
  "  gl_FrontColor = gl_FrontMaterial.diffuse               \n"
  "                  + gl_FrontMaterial.ambient;            \n"
  "  gl_Position = ftransform();                            \n"
  "}                                                        \n";

  GLchar frag_prog_str[] = 
  "void main()                                              \n"
  "{                                                        \n"
  "  gl_FragColor = gl_Color;                               \n"
  "}                                                        \n";
}

void Reflectance::init()
{
  Shader::init(vert_prog_str, frag_prog_str);
}

void Reflectance::set_material(const ObjMaterial* m)
{
  Emission::set_material(m);
  set_diffuse(m);
}

void Reflectance::set_diffuse(const ObjMaterial* m) const
{
  static const float default_diffuse[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
  if(m)
    glMaterialfv(GL_FRONT, GL_DIFFUSE, m->diffuse);
  else
    glMaterialfv(GL_FRONT, GL_DIFFUSE, default_diffuse);
}
