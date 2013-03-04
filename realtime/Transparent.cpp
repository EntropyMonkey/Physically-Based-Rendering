// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#include <GL/glew.h>
#include "ObjMaterial.h"
#include "Transparent.h"

namespace
{
  GLchar vert_prog_str[] = 
  "varying vec3 position;                                   \n"
  "varying vec3 normal;                                     \n"
  "void main()                                              \n"
  "{                                                        \n"
  "  position = (gl_ModelViewMatrix*gl_Vertex).xyz;         \n"
  "  normal = gl_NormalMatrix*gl_Normal;                    \n"
  "  gl_Position = ftransform();                            \n" 
  "}                                                        \n";

  GLchar frag_prog_str[] =
  "uniform samplerCube env;                                 \n"
  "uniform float ior;                                       \n"
  "varying vec3 position;                                   \n"
  "varying vec3 normal;                                     \n"
  "void main()                                              \n"
  "{                                                        \n"
  "  vec3 n = normal*gl_NormalScale;                        \n"

  // Implement the fragment shader for transparent materials here.

  "  gl_FragColor.rgb = n;                                  \n"
  "}                                                        \n";
}

void Transparent::init()
{
  Mirror::init(vert_prog_str, frag_prog_str);
}

void Transparent::set_material(const ObjMaterial* m)
{
  Mirror::set_material(m);
  glUniform1f(glGetUniformLocation(program, "ior"), m->ior);
}
