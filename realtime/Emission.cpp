// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#include <GL/glew.h>
#include "ObjMaterial.h"
#include "Emission.h"

namespace 
{
  GLchar vert_prog_str[] = 
  "void main()                                              \n"
  "{                                                        \n"
  "  gl_FrontColor = gl_FrontMaterial.ambient;              \n"
  "  gl_Position = ftransform();                            \n"
  "}                                                        \n";

  GLchar frag_prog_str[] = 
  "void main()                                              \n"
  "{                                                        \n"
  "  gl_FragColor = gl_Color;                               \n"
  "}                                                        \n";
}

void Emission::init()
{
  Shader::init(vert_prog_str, frag_prog_str);
}

void Emission::set_material(const ObjMaterial* m)
{
  set_ambient(m);
}

void Emission::set_ambient(const ObjMaterial* m) const
{
  static const float default_ambient[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
  if(m)
    glMaterialfv(GL_FRONT, GL_AMBIENT, m->ambient);
  else
    glMaterialfv(GL_FRONT, GL_AMBIENT, default_ambient);
}
