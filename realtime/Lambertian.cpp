// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#include <GL/glew.h>
#include "Lambertian.h"

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

  // Implement the fragment shader for Lambertian reflection here.
  // Ensure that it handles both directional lights and area lights.

  "  gl_FragColor.rgb = n*0.5 + vec3(0.5);                  \n"
  "}                                                        \n";
}

void Lambertian::init()
{
  Shader::init(vert_prog_str, frag_prog_str);
}

void Lambertian::enable()
{
  Shader::enable();
  glEnable(GL_LIGHTING);
  for(unsigned int i = 0; i < lights.size(); ++i)
  {
    lights[i]->set_light(GL_LIGHT0 + i);
    glEnable(GL_LIGHT0 + i);
  }
}

void Lambertian::disable()
{
  for(unsigned int i = 0; i < lights.size(); ++i)
    glDisable(GL_LIGHT0 + i);
  glDisable(GL_LIGHTING);
  Shader::disable();
}
