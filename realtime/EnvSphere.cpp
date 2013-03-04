// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#include <GL/glew.h>
#include "CGLA/Vec3f.h"
#include "CGLA/Vec2f.h"
#include "Camera.h"
#include "EnvSphere.h"

using namespace CGLA;

namespace 
{
  GLchar vert_prog_str[] = 
  "varying vec3 direction;                                  \n"
  "void main()                                              \n"
  "{                                                        \n"
  "  direction = gl_MultiTexCoord0.xyz;                     \n"
  "  gl_Position = ftransform();                            \n"
  "}                                                        \n";

  GLchar frag_prog_str[] = 
  "uniform sampler2D env;                                   \n"
  "varying vec3 direction;                                  \n"
  "void main()                                              \n"
  "{                                                        \n"
  "  vec3 d = normalize(direction);                         \n"

  // Implement the angular map from direction to texture uv-coordinates

  "  gl_FragColor.rgb = d;                                  \n"
  "}                                                        \n";
}

void EnvSphere::init()
{
  Shader::init(vert_prog_str, frag_prog_str);

  glUseProgram(program);
  glUniform1i(glGetUniformLocation(program, "env"), 0);
  glUseProgram(0);
}

void EnvSphere::enable()
{
  Shader::enable();
  if(tex)
  {
    tex->bind();
    tex->enable();
  }
}

void EnvSphere::disable()
{
  if(tex)
    tex->disable();
  Shader::disable();
}

void EnvSphere::draw(const Camera& cam)
{
  if(!tex || !tex->has_texture())
    return;

  static const float far_clip = 1.0e-6f - 1.0f;

  float projection[16];
  float modelview[16];
  glGetFloatv(GL_PROJECTION_MATRIX, projection);
  glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
  
  glMatrixMode(GL_PROJECTION);	 
  glLoadIdentity();             
  glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);  
  glLoadIdentity();

  enable();
  glUniformMatrix4fv(glGetUniformLocation(program, "modelview"), 1, false, modelview);
  glBegin(GL_TRIANGLE_STRIP);
    glTexCoord3fv(cam.get_ray_dir(Vec2f(-0.5f, -0.5f)).get());
    glVertex3f(0.0f, 0.0f, far_clip);
    glTexCoord3fv(cam.get_ray_dir(Vec2f(0.5f, -0.5f)).get());
    glVertex3f(1.0f, 0.0f, far_clip);
    glTexCoord3fv(cam.get_ray_dir(Vec2f(-0.5f, 0.5f)).get());
    glVertex3f(0.0f, 1.0f, far_clip);
    glTexCoord3fv(cam.get_ray_dir(Vec2f(0.5f, 0.5f)).get());
    glVertex3f(1.0f, 1.0f, far_clip);
  glEnd();
  disable();

  glMatrixMode(GL_PROJECTION);	 
  glLoadMatrixf(projection);
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixf(modelview);
}
