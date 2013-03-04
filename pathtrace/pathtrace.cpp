// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2013
// Copyright (c) DTU Compute 2013

#include "RenderEngine.h"

int main(int argc, char** argv)
{
  render_engine.init_GLUT(argc, argv);
  render_engine.load_files(argc, argv);

  render_engine.init_GL();
  render_engine.init_view();
  render_engine.init_tracer();

  glutMainLoop();
  return 0;
}
