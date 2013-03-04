// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2010
// Copyright (c) DTU Informatics 2010

#include <iostream>
#include <GL/glew.h>
#include "CGLA/Vec3f.h"
#include "AreaLight.h"
#include "AABB.h"
#include "mt_random.h"
#include "Shader.h"
#include "Shadow.h"

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
  "uniform float threshold;                                 \n"
  "varying vec3 position;                                   \n"
  "varying vec3 normal;                                     \n"
  "void main()                                              \n"
  "{                                                        \n"
  "  const float M_1_PI = 0.31830988618379067154;           \n"
  "  gl_LightSourceParameters light = gl_LightSource[0];    \n"
  "  vec3 ambient = gl_FrontMaterial.ambient.rgb;           \n"
  "  vec3 n = normal*gl_NormalScale;                        \n"

  // Implement the fragment shader for Lambertian reflection here.
  // In this shader you only need to support area lights.
  // Include shadows using the omnidirectional distance map stored 
  // in the "shadow" cube map.

  "  gl_FragColor.rgb = vec3(0.0);                          \n"
  "}                                                        \n";

  GLchar distance_vert_str[] = 
  "varying vec3 position;                                   \n"
  "void main()                                              \n"
  "{                                                        \n"
  "  position = (gl_ModelViewMatrix*gl_Vertex).xyz;         \n"
  "  gl_Position = ftransform();                            \n"
  "}                                                        \n";

  GLchar distance_frag_str[] = 
  "varying vec3 position;                                   \n"
  "void main()                                              \n"
  "{                                                        \n"

  // Implement distance shader here. Return the squared distance 
  // from eye to fragment position in the red color band.

  "  gl_FragColor.r = 0.0;                                  \n"
  "}                                                        \n";

  class Distance : public Shader
  {
  public:
    virtual void init()
    {
      Shader::init(distance_vert_str, distance_frag_str);
    }

    void init(Scene* scene);

    void draw()
    {
      glCallList(disp_list);      
    }

  protected:
    unsigned int disp_list;
    vector<Shader*> shaders;
  } distance_map;

  void Distance::init(Scene* scene)
  {
    Shader::init(distance_vert_str, distance_frag_str);

    shaders = scene->get_shaders();
    scene->set_shaders(&distance_map);
    scene->gen_disp_list(disp_list);
    scene->set_shaders(shaders);
    scene->redo_display_list();      
  }

  void draw()
  {
    distance_map.draw();
  }
}

void Shadow::init(const char* vert_shader, const char* frag_shader)
{
  if(lights.size() < light_idx + 1)
  {
    cerr << "Error: Light was not added to scene before initialization of omnidirectional shadow map" << endl;
    exit(1);
  }
  light = dynamic_cast<const AreaLight*>(lights[light_idx]);
  if(!light)
    cerr << "Warning: Light chosen for omnidirectional shadow map is not an area light" << endl;

  cube_map.init();
  Shader::init(vert_shader, frag_shader);

  const AABB& bbox = scene->get_bbox();
  Vec3f dimensions = bbox.get_diagonal();
  float threshold = dot(dimensions, dimensions)*1.0e-3f;
  glUseProgram(program);
  glUniform1f(glGetUniformLocation(program, "threshold"), threshold);
  glUniform1i(glGetUniformLocation(program, "shadow"), 0);
  glUseProgram(0);
}

void Shadow::init()
{
  init(vert_prog_str, frag_prog_str);
}

void Shadow::generate_map()
{
  static bool first = true;
  if(first)
  {
    first = false;
    distance_map.init(scene);
  }
  if(light)
    cube_map.generate(draw, light->get_mesh());
}

void Shadow::enable()
{
  Lambertian::enable();
  cube_map.enable();
}

void Shadow::disable()
{
  cube_map.disable();
  Lambertian::disable();
}
