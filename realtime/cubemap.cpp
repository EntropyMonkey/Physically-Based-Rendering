// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#include <GL/glew.h>
#include "CGLA/Vec3f.h"
#include "TriMesh.h"
#include "Scene.h"
#include "CubeMap.h"

using namespace std;
using namespace CGLA;

void CubeMap::init()
{
  glGenTextures(1, &cube);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cube);

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  for(int face = 0; face < 6; ++face)
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, iformat, size, size, 0, format, GL_FLOAT, 0);

  glGenFramebuffersEXT(1, &fbo);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
  for(int face = 0; face < 6; ++face)
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + face, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, cube, 0);

  glGenRenderbuffersEXT(1, &depth_rb);
  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_rb);
  glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, size, size);
  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
  glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depth_rb);

  GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
  switch(status) 
  {
  case GL_FRAMEBUFFER_COMPLETE_EXT:
    break;                                               
  case GL_FRAMEBUFFER_UNSUPPORTED_EXT:     
    cerr << "Warning: cube map framebuffer configuration unsupported" << endl;
    break;                                               
  default:                                               
    cerr << "Error: cube map initialization failed" << endl;   

    GLenum glErr = glGetError();
    fprintf(stderr, "glError: %s\n", gluErrorString(glErr));
    exit(0);                                           
  }
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void CubeMap::init(unsigned int face_size, GLenum tex_iformat, GLenum tex_format)
{
  size = face_size;
  iformat = tex_iformat;
  format = tex_format;
  init();
}

void CubeMap::erase()
{
  glDeleteTextures(1, &cube);
  glDeleteFramebuffersEXT(1, &fbo);
  glDeleteRenderbuffersEXT(1, &depth_rb);
}

void CubeMap::enable() const
{
  glBindTexture(GL_TEXTURE_CUBE_MAP, cube);
  glEnable(GL_TEXTURE_CUBE_MAP);
}

void CubeMap::disable() const
{
  glDisable(GL_TEXTURE_CUBE_MAP);
}

void CubeMap::before_draw(int face) const
{
  // The face variable specifies the viewing direction
  Vec3f look_at = center;
  Vec3f up(0.0f);  
  switch(face)
  {
  case 0: case 1:
  case 4: case 5:
    look_at[face/2] += 1.0f - 2.0f*(face % 2);
    up[1] = -1.0f;
    break;
  case 2:
    look_at[1] += 1.0f;
    up[2] =  1.0f; 
    break;
  case 3:
    look_at[1] -= 1.0f;
    up[2] = -1.0f; 
    break;
  default:
    return;
  }

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
  glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT + face);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // 90-degrees field of view and a square aspect ratio,
  // so the six cube faces seam up tightly
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(90.0, 1.0, radius, 1.0e8);
  glMatrixMode(GL_MODELVIEW);

  glLoadIdentity();
  gluLookAt(center[0], center[1], center[2],
            look_at[0], look_at[1], look_at[2],
            up[0], up[1], up[2]);
}

//////////////////////////////////////////////////////////
//   Using a scene pointer for drawing
//////////////////////////////////////////////////////////

void CubeMap::snapshot(Scene* s, int face) const
{
  before_draw(face);
  s->draw();
}

void CubeMap::generate(Scene* s, const TriMesh* object)
{
  // Object bounding sphere determines the position of the cube map
  object->compute_bsphere(center, radius);
  radius *= 0.5;

  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);
  glViewport(0, 0, size, size);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  for(int i = 0; i < 6; ++i)
    snapshot(s, i);

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);

  glViewport(vp[0], vp[1], vp[2], vp[3]);
}

//////////////////////////////////////////////////////////
//   Using a callback function for drawing
//////////////////////////////////////////////////////////

void CubeMap::snapshot(void (*callback)(), int face) const
{ 
  before_draw(face);
  callback();
}

void CubeMap::generate(void (*callback)(), const TriMesh* object)
{
  // Object bounding sphere determines the position of the cube map
  object->compute_bsphere(center, radius);
  radius *= 0.5;

  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);
  glViewport(0, 0, size, size);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  for(int i = 0; i < 6; ++i)
    snapshot(callback, i);

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);

  glViewport(vp[0], vp[1], vp[2], vp[3]);
}
