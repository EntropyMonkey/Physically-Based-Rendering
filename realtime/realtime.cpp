// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2010
// Copyright (c) DTU Informatics 2010

#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <GL/glew.h>
#ifdef WIN32
#include <GL/wglew.h>
#endif
#include <GL/glut.h>
#include "CGLA/Vec3f.h"
#include "CGLA/Mat4x4f.h"
#include "GLGraphics/GLViewController.h"
#include "GLGraphics/SOIL.h"
#include "Util/string_utils.h"
#include "Timer.h"
#include "Camera.h"
#include "Scene.h"
#include "Directional.h"
#include "SHDirectional.h"
#include "SHOmni.h"
#include "Shader.h"
#include "Reflectance.h"
#include "Lambertian.h"
#include "Glossy.h"
#include "Shadow.h"
#include "SoftShadow.h"
#include "Mirror.h"
#include "Transparent.h"
#include "Metal.h"
#include "RadianceTransfer.h"
#include "SphereTexture.h"
#include "EnvSphere.h"

using namespace std;
using namespace CGLA;
using namespace GLGraphics;

//////////////////////////////////////////////////////////////////////
// Globals
//////////////////////////////////////////////////////////////////////

namespace
{
  int winw = 512, winh = 512;

  // View controller
  int mouse_state = GLUT_UP;
  int spin_timer = 20;
  GLViewController* vctrl;
  Camera cam;

  // Frame rate measurement
  bool find_frame_rate = false;
  double fps = 0.0;

  // Geometry container
  Scene scene(&cam);
  string filename;

  // Light
  Vec3f light_pow(static_cast<float>(M_PI));
  Vec3f light_dir = normalize(Vec3f(-1.0f, -1.0f, -1.0f));
  Directional default_light(light_pow, light_dir);
  unsigned int no_of_bands = 2;
  SHDirectional sh_default(no_of_bands, default_light);

  // Environment
  Vec3f background(0.34f, 0.55f, 0.85f);
  SphereTexture bgtex;
  string bgtex_filename("");
  EnvSphere env_sphere(&bgtex);
  SHOmni sh_omni(no_of_bands, 10000, background, &bgtex);

  // Shaders
  unsigned int current_shader = 0;
  Reflectance reflectance;
  Lambertian lambertian(scene.get_lights());
  Glossy glossy(scene.get_lights());
  Shadow shadow(&scene);
  SoftShadow soft_shadow(&scene, 5); // Change 5 to set the number of soft shadow samples
  Mirror mirror(&scene);
  Transparent transparent(&scene);
  Metal metal(&scene);
  RadianceTransfer prt_default("scene.prt", &sh_default, 2);
  RadianceTransfer prt_omni("scene.prt", &sh_omni, 2);
  Shader* shaders[] = { &reflectance,     // number key 0 (reflectance only)
                        &lambertian,      // number key 1 (local lighting)
                        &shadow,          // number key 2 (omnidirectional shadow mapping)
                        &soft_shadow,     // number key 3 (interactive soft shadows)
                        &prt_default,     // number key 4 (PRT with default light)
                        &prt_omni     };  // number key 5 (PRT with environment lighting)

  unsigned int no_of_shaders()
  {
    return sizeof(shaders)/sizeof(Shader*);
  }

  // String utilities
	void lower_case(char& x) { x = tolower(x); }

	inline void lower_case_string(std::string& s)
	{
    for_each(s.begin(), s.end(), lower_case);
	}
}

void display();
void spin(int x);

//////////////////////////////////////////////////////////////////////
// Initialization
//////////////////////////////////////////////////////////////////////

void init_GL()
{
  glEnable(GL_CULL_FACE);

  int no_of_sample_bufs;
  glGetIntegerv(GL_SAMPLE_BUFFERS, &no_of_sample_bufs);
  glDisable(GL_MULTISAMPLE); // freeglut switches multisampling on per default with GLUT_MULTISAMPLE
  if(!no_of_sample_bufs)
    cout << "Warning: Hardware multisampling is not supported "
         << "(some versions of GLUT do not support multisampling even if the hardware does)." << endl;

  // glew initialization
  GLenum err = glewInit();
  if(GLEW_OK != err)
  {
    cout << "Error: " << glewGetErrorString(err);
    exit(1);
  }

  if(!GLEW_ARB_shader_objects)
  {
  	cerr << "Shader objects are not supported.";
    return;
  }
  if(!GLEW_ARB_fragment_program)
  {
    cerr << "Fragment shaders are not supported.";
    return;
  }
}

void init_view()
{
  Vec3f c;
  float r;
  scene.get_bsphere(c, r);
  r *= 1.75f;

  // Initialize track ball
  vctrl = new GLViewController(winw, winh, c, r);

  // Initialize corresponding camera for tracer
  cam.set(c - Vec3f(r), c, Vec3f(0.0f, 0.0f, 1.0f), 1.0f);
}

void load_shaders()
{
  for(unsigned int i = 0; i < no_of_shaders(); ++i)
    shaders[i]->init();
  glossy.init();
  mirror.init();
  transparent.init();
  metal.init();
}

void init_shaders()
{
  // Load background texture
  if(!bgtex_filename.empty())
  {
    bgtex.load_hdr(bgtex_filename.c_str());
    env_sphere.init();
  }

  // Add polygons with an ambient material as area light sources
  unsigned int lights_in_scene = scene.extract_area_lights();

  // If no light in scene, add default light source (shadow off)
  if(lights_in_scene == 0)
    scene.add_light(&default_light);

  // Set shaders for the different illumination models
  load_shaders();
  scene.set_shader(0, shaders[current_shader]); // shader for illum 0 (chosen by number key)
  scene.set_shader(1, shaders[current_shader]); // shader for illum 1 (chosen by number key)
  scene.set_shader(2, &glossy);                 // shader for illum 2
  scene.set_shader(3, &mirror);                 // shader for illum 3
  scene.set_shader(4, &transparent);            // shader for illum 4
  scene.set_shader(11, &metal);                 // shader for illum 11
  scene.load_media("..\\models\\media.mpml");

  // Extract objects with a specular material
  scene.extract_specular_objs();
}

//////////////////////////////////////////////////////////////////////
// Draw functions
//////////////////////////////////////////////////////////////////////

void set_ortho_proj()
{
  glMatrixMode(GL_PROJECTION);	 
  glLoadIdentity();             
    
  glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);

  glMatrixMode(GL_MODELVIEW);  
}

//////////////////////////////////////////////////////////////////////
// GLUT Callback Functions
//////////////////////////////////////////////////////////////////////

void display()
{
  static FrameRateTimer t;
  static bool first = true;
  if(first)
  {
    first = false;
    glutTimerFunc(spin_timer, spin, 0);
  }

  Vec3f eye, focus, up;
	vctrl->get_view_param(eye, focus, up);
	cam.set(eye, focus, up, cam.get_focal_dist());

  glEnable(GL_DEPTH_TEST);

  if(shaders[current_shader] == &shadow)
    shadow.generate_map();
  if(shaders[current_shader] == &soft_shadow)
    soft_shadow.generate_map();
  scene.draw_reflection_maps();

  cam.glSetPerspective(winw, winh);

  glClearColor(background[0], background[1], background[2], 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  cam.glSetCamera();
  env_sphere.draw(cam);
  scene.draw();

  glDisable(GL_DEPTH_TEST);

  if(find_frame_rate)
  {
    ostringstream ostr;
    fps = t.get_fps();
    ostr << "Real-time shading (fps: " << fps << ")";
    glutSetWindowTitle(ostr.str().c_str());
  }

  glutSwapBuffers();  
}

void reshape(int width, int height)
{
  winw = width;
  winh = height;

  glViewport(0, 0, winw, winh);
  vctrl->reshape(winw, winh);
}

void keyboard(unsigned char key, int x, int y)
{
  // The shader to be used when rendering a material is chosen 
  // by setting the "illum" property of the material. This
  // property is part of the MTL file format ("illum" is short
  // for "illumination model"). The shader to be used with
  // each illumination model is specified in the init_tracer 
  // function above.
  //
  // Number keys switch the shader used for illumination
  // models 0 and 1 to the shader at the corresponding index
  // in the array "shaders" at the top of this file.
  if(key >= 48 && key < 48 + static_cast<unsigned char>(no_of_shaders()))
  {
    unsigned int shader_no = key - 48;
    if(shader_no != current_shader)
    {
      current_shader = shader_no;
      for(int i = 0; i < 2; ++i)
        scene.set_shader(i, shaders[current_shader]);
      scene.redo_display_list();
      cout << "Switched to shader number " << shader_no << endl;
      if(shaders[current_shader] == &prt_omni)
      {
        cout << "Computing spherical harmonics coefficients for environment lighting.." << endl;
        sh_omni.compute_sh();
        prt_omni.update_light();
      }
      glutPostRedisplay();
    }
  }
  switch(key)
  {
  // Press 'a' to toggle hardware multisample antialiasing on/off
  case 'a':
    if(glIsEnabled(GL_MULTISAMPLE))
      glDisable(GL_MULTISAMPLE);
    else
      glEnable(GL_MULTISAMPLE);
    glutPostRedisplay();
    break;
  // Press 'b' to save a bitmap called the same as the last loaded .obj file.
  case 'b':
    {
      list<string> dot_split;
      Util::split(filename, dot_split, ".");
      string bmp_name = dot_split.front() + ".bmp";
      GLint read_buffer;
      glGetIntegerv(GL_READ_BUFFER, &read_buffer);
      glReadBuffer(GL_FRONT);
      SOIL_save_screenshot(bmp_name.c_str(), SOIL_SAVE_TYPE_BMP, 0, 0, winw, winh);
      glReadBuffer(read_buffer);
      cout << "Screenshot stored in " << bmp_name << "." << endl;
    }
    break;
  // Press 'f' to switch frame rate measurement on/off
  case 'f':
    find_frame_rate = !find_frame_rate;
#ifdef WIN32
    if(find_frame_rate)
      wglSwapIntervalEXT(0);
    else
    {
      wglSwapIntervalEXT(1);
      glutSetWindowTitle("Real-time shading");
    }
    break;
#else
    if(!find_frame_rate)
      glutSetWindowTitle("Real-time shading");
#endif      
  // Press 'L' to load a view saved in the file "view".
  case 'L':
    {
      ifstream ifs("view");
      vctrl->load(ifs);
      cout << "View loaded from file: view" << endl;

      Vec3f eye, lookat, up;
      vctrl->get_view_param(eye, lookat, up);
      cout << "Eye   : " << eye << endl
	         << "Lookat: " << lookat << endl
	         << "Up    : " << up << endl;
      glutPostRedisplay();
    }
    break;
  // Press 'S' to save the current view in the file "view".
  case 'S':
    {
      ofstream ofs("view");
      vctrl->save(ofs);
      cout << "Current view stored in file: view" << endl;
    }
    break;
  // Press 'r' to reload all shaders.
  case 'r':
    load_shaders();
    scene.redo_display_list();
    glutPostRedisplay();
    break;
  // Press 'ESC' to exit the program.
  case 27:
    delete vctrl;
    exit(0);
  }
}

void special(int key, int x, int y)
{
  double theta = acos(light_dir[2]);
  double phi = atan2(light_dir[1], light_dir[0]);
  phi =  phi < 0.0 ? phi + 2.0*M_PI : phi;

  switch(key)
  {
  case GLUT_KEY_UP:    theta += 0.1; break;
  case GLUT_KEY_DOWN:  theta -= 0.1; break;
  case GLUT_KEY_LEFT:  phi += 0.1;   break;
  case GLUT_KEY_RIGHT: phi -= 0.1;   break;
  }
  if(theta < 0.0)
    theta = 0.0;
  else if(theta > M_PI)
    theta = M_PI;

  light_dir = Vec3f(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));
  default_light.set_direction(light_dir);
  sh_default = SHDirectional(no_of_bands, default_light);
  sh_default.compute_sh();
  prt_default.update_light();
  glutPostRedisplay();
}

void mouse(int btn, int state, int x, int y)
{
  if(state == GLUT_DOWN) 
  {
    if(btn == GLUT_LEFT_BUTTON) 
      vctrl->grab_ball(ROTATE_ACTION, Vec2i(x, y));
    else if(btn == GLUT_MIDDLE_BUTTON) 
      vctrl->grab_ball(ZOOM_ACTION, Vec2i(x, y));
    else if(btn == GLUT_RIGHT_BUTTON) 
      vctrl->grab_ball(PAN_ACTION, Vec2i(x, y));
  }
  else if(state == GLUT_UP)
			vctrl->release_ball();

  mouse_state = state;
}

void move(int x, int y)
{
  vctrl->roll_ball(Vec2i(x, y));
  if(mouse_state == GLUT_DOWN)
    glutPostRedisplay();
}

void spin(int x)
{
  if(vctrl->try_spin())
    glutPostRedisplay();
  glutTimerFunc(spin_timer, spin, 0);  
}

void idle()
{
  if(find_frame_rate)
    glutPostRedisplay();
}

//////////////////////////////////////////////////////////////////////
// Main
//////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
  glutInitWindowSize(winw, winh);
  glutCreateWindow("Real-time shading");
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(special);
  glutMouseFunc(mouse);
  glutMotionFunc(move);
  glutIdleFunc(idle);

  // Load .obj-file
  if(argc > 1)
  {
    for(int i = 1; i < argc; ++i)
    {
      list<string> path_split;
      Util::split(argv[i], path_split, "\\");
      filename = path_split.back();
      if(filename.find("/") != filename.npos)
      {
        path_split.clear();
        Util::split(filename, path_split, "/");
        filename = path_split.back();
      }
      lower_case_string(filename);
      Mat4x4f transform = identity_Mat4x4f();
      if(char_traits<char>::compare(filename.c_str(), "cornell", 7) == 0)
        transform = scaling_Mat4x4f(Vec3f(0.025f))*rotation_Mat4x4f(YAXIS, static_cast<float>(M_PI));
      else if(char_traits<char>::compare(filename.c_str(), "bunny", 5) == 0)
        transform = translation_Mat4x4f(Vec3f(-3.0f, -0.85f, -8.0f))*scaling_Mat4x4f(Vec3f(25.0f));
      else if(char_traits<char>::compare(filename.c_str(), "justelephant", 12) == 0)
        transform = translation_Mat4x4f(Vec3f(-10.0f, 3.0f, -2.0f))*rotation_Mat4x4f(YAXIS, 0.5f);
      scene.load_mesh(argv[i], transform);
    }
  }
  else
  {
    cout << "Usage: realtime any_object.obj [another.obj ...]" << endl;
    exit(0);
  }

  // Call initializers
  init_GL();
  init_view();
  init_shaders();
  
  glutMainLoop();
  return 0;
}
