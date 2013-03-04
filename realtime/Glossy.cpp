// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2010
// Copyright (c) DTU Informatics 2010

#include <GL/glew.h>
#include "ObjMaterial.h"
#include "Glossy.h"

namespace 
{
  GLchar vert_prog_str[] = 
  "varying vec3 position;                                   \n"
  "varying vec3 normal;                                     \n"
  "void main()                                              \n"
  "{                                                        \n"
  "  gl_FrontColor = gl_FrontMaterial.diffuse;              \n"
  "  position = (gl_ModelViewMatrix*gl_Vertex).xyz;         \n"
  "  normal = gl_NormalMatrix*gl_Normal;                    \n"
  "  gl_Position = ftransform();                            \n"
  "}                                                        \n";

  GLchar frag_prog_str[] = 
  "varying vec3 position;                                   \n"
  "varying vec3 normal;                                     \n"
  "void main()                                              \n"
  "{                                                        \n"
  "  const float M_1_PI = 0.31830988618379067154;           \n"
  "  vec3 n = normal*gl_NormalScale;                        \n"
  "  gl_LightSourceParameters light = gl_LightSource[0];    \n"
  "  vec3 ambient = gl_FrontMaterial.ambient.rgb;           \n"
  "  vec3 rho_d = gl_FrontMaterial.diffuse.rgb;             \n"
  "  vec3 rho_s = gl_FrontMaterial.specular.rgb;            \n"
  "  float s = gl_FrontMaterial.shininess;                  \n"

  // Implement the fragment shader for glossy reflection here.
  // You can assume that there is only one light source.

  "  gl_FragColor.rgb = n;                                  \n"
  "}                                                        \n";
}

void Glossy::init()
{
  Shader::init(vert_prog_str, frag_prog_str);
}

void Glossy::set_material(const ObjMaterial* m)
{
  Reflectance::set_material(m);
  set_specular(m);
  set_shininess(m);
}

void Glossy::set_specular(const ObjMaterial* m) const
{
  static const float default_specular[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
  if(m)
    glMaterialfv(GL_FRONT, GL_SPECULAR, m->specular);
  else
    glMaterialfv(GL_FRONT, GL_SPECULAR, default_specular);
}

void Glossy::set_shininess(const ObjMaterial* m) const
{
  static const float default_shininess = 0.0f;
  if(m)
    glMaterialf(GL_FRONT, GL_SHININESS, m->shininess);
  else
    glMaterialf(GL_FRONT, GL_SHININESS, default_shininess);
}

