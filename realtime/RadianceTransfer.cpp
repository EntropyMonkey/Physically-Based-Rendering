// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#include <string>
#include <sstream>
#include <valarray>
#include <GL/glew.h>
#include "CGLA/Vec3f.h"
#include "TriMesh.h"
#include "RadianceTransfer.h"

using namespace std;
using namespace CGLA;

namespace 
{
  GLchar vert_prog_str[] = 
  "attribute vec3 c0;                                       \n"
  "attribute vec3 c1;                                       \n"
  "attribute vec3 c2;                                       \n"
  "attribute vec3 c3;                                       \n"
  "uniform vec3 light[4];                                   \n"
  "void main()                                              \n"
  "{                                                        \n"
  
  // Reconstruct lighting and store result in gl_FrontColor
  "  gl_FrontColor.rgb = vec3(0.0);                         \n"
  "  gl_Position = ftransform();                            \n"
  "}                                                        \n";

  GLchar frag_prog_str[] = 
  "void main()                                              \n"
  "{                                                        \n"
  "  gl_FragColor.rgb = gl_Color.rgb;                       \n"
  "}                                                        \n";
}

void RadianceTransfer::init()
{
  Shader::init(vert_prog_str, frag_prog_str);

  int max_attribs;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attribs);
  if(bands*bands > static_cast<unsigned int>(max_attribs))
    cerr << "warning: the number of spherical harmonics coefficients "
         << "exceeds the maximum number of vertex attributes." << endl;

  update_light();
}

void RadianceTransfer::set_attributes(const TriMesh* mesh, int index)
{
  if(!fin.eof() && coeffs.count(index) == 0)
    load_next_coeffs(mesh, index);
  const valarray<Vec3f>& attribs = coeffs[index + idx_counter];

  // Upload coefficients as vertex attributes
  for(unsigned int i = 0; i < attribs.size(); ++i)
  {
    ostringstream ostr;
    ostr << "c" << i;
    string attrib_name = ostr.str();
    glVertexAttrib3fv(glGetAttribLocation(program, attrib_name.c_str()), attribs[i].get());
  }
}

void RadianceTransfer::update_light()
{
  enable();
  light->upload_sh(program);
  disable();
}

void RadianceTransfer::load_next_coeffs(const TriMesh* mesh, int index)
{
  if(!current_mesh)
    current_mesh = mesh;
  if(mesh != current_mesh)
  {
    idx_counter += current_mesh->geometry.no_vertices();
    current_mesh = mesh;
  }

  valarray<Vec3f>& attribs = coeffs[index + idx_counter];
  attribs.resize(bands*bands);
  for(unsigned int i = 0; i < attribs.size(); ++i)
    fin.read(reinterpret_cast<char*>(attribs[i].get()), sizeof(Vec3f));
}
