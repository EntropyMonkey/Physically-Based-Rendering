// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#include <GL/glew.h>
#include <valarray>
#include <complex>
#include "ObjMaterial.h"
#include "Metal.h"

using namespace std;

namespace
{
  GLchar vert_prog_str[] = 
  "varying vec4 position;                                   \n"
  "varying vec3 normal;                                     \n"
  "varying vec3 world_pos;                                  \n"
  "void main()                                              \n"
  "{                                                        \n"
  "  world_pos = gl_Vertex.xyz;                             \n"
  "  position = gl_ModelViewMatrix*gl_Vertex;               \n"
  "  normal = gl_NormalMatrix*gl_Normal;                    \n"
  "  gl_Position = ftransform();                            \n" 
  "}                                                        \n";

  GLchar frag_prog_str[] =
  // Fresnel for conductors
  "float fresnel_R(vec2 ior, float cos_theta)               \n"
  "{                                                        \n"
  "  return 0.5;                                            \n"
  "}                                                        \n"
  // End of Fresnel computation

  "uniform samplerCube env;                                 \n"
  "uniform vec3 env_center;                                 \n"
  "uniform vec2 ior[3];                                     \n"
  "varying vec4 position;                                   \n"
  "varying vec3 normal;                                     \n"
  "varying vec3 world_pos;                                  \n"
  "void main()                                              \n"
  "{                                                        \n"
  "  vec3 n = normal*gl_NormalScale;                        \n"

  // Implement the fragment shader for metalic reflection here.
  // It might be advantageous to implement the fresnel_R function above.

  "  gl_FragColor.rgb = n;                                  \n"
  "}                                                        \n";
}

void Metal::init()
{
  Mirror::init(vert_prog_str, frag_prog_str);
}

void Metal::set_material(const ObjMaterial* m)
{
  float float_ior[6];
  const valarray< complex<double> >& ior = scene->get_medium(m).get_ior(rgb);
  for(unsigned int i = 0; i < 6; i += 2)
  {
    float_ior[i] = ior[i/2].real();
    float_ior[i + 1] = ior[i/2].imag();
  }
  glUniform2fv(glGetUniformLocation(program, "ior"), 3, float_ior);
}
