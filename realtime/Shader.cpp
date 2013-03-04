// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2010
// Copyright (c) DTU Informatics 2010

#include <string>
#include <GL/glew.h>
#include "GLGraphics/glsl_shader.h"
#include "Shader.h"

using namespace std;
using namespace GLGraphics;

void Shader::init(const char* vert_shader, const char* frag_shader)
{
  GLuint vs = create_glsl_shader(GL_VERTEX_SHADER, vert_shader);
  GLuint fs = create_glsl_shader(GL_FRAGMENT_SHADER, frag_shader); 

  if(vs || fs)
  {
    program = glCreateProgram();
    if(vs) glAttachShader(program, vs);
    if(fs) glAttachShader(program, fs);
    glLinkProgram(program);
    print_glsl_program_log(program);
  }
}

void Shader::init(const char* path, const char* vert_file, const char* frag_file)
{
  if(vert_file)
  {
    string vert_shader = read_glsl_source(path, vert_file);
    if(frag_file)
    {
      string frag_shader = read_glsl_source(path, frag_file);
      init(vert_shader.c_str(), frag_shader.c_str());
    }
    else
      init(vert_shader.c_str());
  }
}

void Shader::enable()
{
  glUseProgram(program);
}

void Shader::disable()
{
  glUseProgram(0);
}
