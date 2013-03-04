// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2010
// Copyright (c) DTU Informatics 2010

#include <vector>
#include <GL/glew.h>
#include "CGLA/Vec3f.h"
#include "SoftShadow.h"

using namespace std;
using namespace CGLA;

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
  "uniform samplerCube shadow;                              \n"
  "uniform sampler1D pos_map;                               \n"
  //"uniform sampler1D norm_map;                              \n"
  "uniform int samples;                                     \n"
  "uniform float threshold;                                 \n"
  "varying vec3 position;                                   \n"
  "varying vec3 normal;                                     \n"
  "void main()                                              \n"
  "{                                                        \n"
  "  const float M_1_PI = 0.31830988618379067154;           \n"
  "  gl_LightSourceParameters light = gl_LightSource[0];    \n"
  "  vec3 ambient = gl_FrontMaterial.ambient.rgb;           \n"
  "  vec3 n = normal*gl_NormalScale;                        \n"

  // Copy your shadow shader here and change it such that a number of
  // samples across the area light are included to obtain soft shadows.
  // We assume that the area light is plane. This is the reason why 
  // the map of sampled surface normals has been commented out.

  "  gl_FragColor.rgb = vec3(0.0);                          \n"
  "}                                                        \n";
}

void SoftShadow::init()
{
  Shadow::init(vert_prog_str, frag_prog_str);

  vector<Vec3f> positions(samples);
  vector<Vec3f> normals(samples);
  if(light)
    for(unsigned int i = 0; i < samples; ++i)
      light->sample(positions[i], normals[i]);

  glGenTextures(2, sample_map);

  glBindTexture(GL_TEXTURE_1D, sample_map[0]);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB16F_ARB, samples, 0, GL_RGB, GL_FLOAT, &positions[0]);

  //glBindTexture(GL_TEXTURE_1D, sample_map[1]);
  //glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  //glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  //glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB16F_ARB, samples, 0, GL_RGB, GL_FLOAT, &normals[0]);

  float threshold = 0.0f;
  int threshold_loc = glGetUniformLocation(program, "threshold");
  if(threshold_loc != -1)
    glGetUniformfv(program, threshold_loc, &threshold);

  glUseProgram(program);
  glUniform1f(threshold_loc, threshold*100.0f);
  glUniform1i(glGetUniformLocation(program, "pos_map"), 1);
  //glUniform1i(glGetUniformLocation(program, "norm_map"), 2);
  glUniform1i(glGetUniformLocation(program, "samples"), samples);
  glUseProgram(0);
}

void SoftShadow::enable()
{
  Shadow::enable();
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_1D, sample_map[0]);
  glEnable(GL_TEXTURE_1D);
  //glActiveTexture(GL_TEXTURE2);
  //glBindTexture(GL_TEXTURE_1D, sample_map[1]);
  //glEnable(GL_TEXTURE_1D);
  glActiveTexture(GL_TEXTURE0);
}

void SoftShadow::disable()
{
  //glActiveTexture(GL_TEXTURE2);
  //glDisable(GL_TEXTURE_1D);
  glActiveTexture(GL_TEXTURE1);
  glDisable(GL_TEXTURE_1D);
  glActiveTexture(GL_TEXTURE0);
  Shadow::disable();
}
