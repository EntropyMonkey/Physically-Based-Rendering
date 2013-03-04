// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#include <GL/glew.h>
#include "TriMesh.h"
#include "Mirror.h"

namespace
{
  GLchar vert_prog_str[] = 
  "varying vec3 position;                                   \n"
  "varying vec3 normal;                                     \n"
  "varying vec3 world_pos;                                  \n"
  "void main()                                              \n"
  "{                                                        \n"
  "  world_pos = gl_Vertex.xyz;                             \n"
  "  position = (gl_ModelViewMatrix*gl_Vertex).xyz;         \n"
  "  normal = gl_NormalMatrix*gl_Normal;                    \n"
  "  gl_Position = ftransform();                            \n" 
  "}                                                        \n";

  GLchar frag_prog_str[] =
  "uniform samplerCube env;                                 \n"
  "uniform vec3 env_center;                                 \n"
  "varying vec3 position;                                   \n"
  "varying vec3 normal;                                     \n"
  "varying vec3 world_pos;                                  \n"
  "void main()                                              \n"

  "{                                                        \n"
  "  vec3 n = normal*gl_NormalScale;                        \n"

  // Implement the fragment shader for mirror reflection here.
  // (optional) Include parallax correction if time permits.

  "  gl_FragColor.rgb = n;                                  \n"
  "}                                                        \n";
}

void Mirror::init(const char* vert_shader, const char* frag_shader)
{
  Shader::init(vert_shader, frag_shader);

  glUseProgram(program);
  glUniform1i(glGetUniformLocation(program, "env"), 0);
  glUseProgram(0);
}

void Mirror::init()
{
  init(vert_prog_str, frag_prog_str);
}

void Mirror::generate_map(const TriMesh* mesh)
{
  current_map = &cube_maps[mesh];
  if(!glIsTexture(current_map->get_handle()))
  {
    current_map->init(128, GL_RGBA16F_ARB, GL_RGBA);
    scene->redo_display_list();
  }
  current_map->generate(scene, mesh);
}

void Mirror::enable()
{
  Shader::enable();
  if(current_map) 
  {
    current_map->enable();
    glUniform3fv(glGetUniformLocation(program, "env_center"), 1, current_map->get_center().get());
  }
}

void Mirror::disable()
{
  if(current_map) 
    current_map->disable();
  Shader::disable();
}
