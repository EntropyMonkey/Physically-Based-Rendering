// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2013
// Copyright (c) DTU Compute 2013

#include <iostream>
#include <algorithm>
#include <list>
#include <map>
#include <string>
#include "GL/glew.h"
#include "GL/glut.h"
#include "CGLA/Vec4f.h"
#include "CGLA/Vec3f.h"
#include "CGLA/Vec2f.h"
#include "CGLA/Vec2i.h"
#include "CGLA/Quatf.h"
#include "GLGraphics/stb_image_write.h"
#include "Util/string_utils.h"
#include "../optprops/Interface.h"
#include "../optprops/Medium.h"
#include "../optprops/water.h"
#include "../optprops/glass.h"
#include "../optprops/milk.h"
#include "Timer.h"
#include "mt_random.h"
#include "ImageCompare.h"
#include "TriMesh.h"
#include "RenderEngine.h"

#ifdef _OPENMP
  #include <omp.h>
#endif

#ifndef M_PIf
#define M_PIf 3.14159265358979323846f
#endif

using namespace std;
using namespace CGLA;
using namespace GLGraphics;

RenderEngine render_engine;

namespace
{
  // String utilities
  void lower_case(char& x) { x = tolower(x); }

  inline void lower_case_string(std::string& s)
  {
    for_each(s.begin(), s.end(), lower_case);
  }
}

//////////////////////////////////////////////////////////////////////
// Initialization
//////////////////////////////////////////////////////////////////////

RenderEngine::RenderEngine() 
  : winw(512), winh(512),                                      // Default window size
    resx(512), resy(512),                                      // Default render resolution
    image(resx*resy, Vec3f(0.0f)),
    image_tex(0),
    sample_number(0.0f),
    split_time(0.0),
    mouse_state(GLUT_UP),
    spin_timer(20),
    vctrl(0),
    scene(&cam),
    tracer(resx, resy, &scene, 100000),                        // Maximum number of photons in map
    max_to_trace(500000),                                      // Maximum number of photons to trace
    caustics_particles(0),                                     // Desired number of caustics photons
    global_particles(0),                                       // Desired number of global particles
    caustics_photons_in_estimate(250),                         // Number of nearest neighbors in caustics estimates
    global_photons_in_estimate(1000),                          // Number of nearest neighbors in global estimates
    look_up_radius(1.0e3f),                                    // Should be large for kth nearest neighbor photon mapping
    start_raytrace(false),
    tracing(false),
    done(false),
    shadows_on(true),
    use_default_light(false),                                   // Choose whether to use the default light or not
    light_pow(Vec3f(M_PIf)),                                    // Power of the default light
    light_dir(normalize(Vec3f(-1.0f))),                         // Direction of the default light
    default_light(&tracer, light_pow, light_dir),

    use_sun_and_sky(true),                                      // Choose whether to use a sky model or not
    background(Vec3f(0.8f, 0.9f, 1.0f)),                       // Background color
    bgtex_filename(""),                                        // Background texture file name
    day_of_year(255.0),                                        // Day in the year counting from 1 January
    time_of_day(12),                                         // Hour in the day where 12 is solar noon
    latitude(55.77),                                           // Angular distance in degrees measured from equator
    world_up(0.0f, 1.0f, 0.0f),                                // The world's up direction
    angle_with_south(0.0),                                     // Angle of the world z-axis wrt. south
    turbidity(2.4),                                            // Atmospheric turbidity (2 is clear sky)
    sun_sky(&tracer),
    
    prt_filename("scene.prt"),
    current_shader(0),
    lambertian(scene.get_lights()),
    glossy(scene.get_lights()),
    ambient(&tracer, scene.get_lights(), 5),                   // No. of samples per pass in ambient occlusion
    mc_lambertian(&tracer, scene.get_lights(), 1),             // No. of samples per pass in path tracing of diffuse materials
    mc_glossy(&tracer, scene.get_lights(), 1),                 // No. of samples per pass in path tracing of glossy materials
    photon_caustics(&tracer, scene.get_lights(), look_up_radius, caustics_photons_in_estimate),
    photon_lambertian(&tracer, scene.get_lights(), look_up_radius, global_photons_in_estimate, true, 3, &photon_caustics),
    mirror(&tracer, 20),                                       // Max. trace depth in rendering of perfect mirrors
    metal(&tracer, 2),                                         // Trace depth at which to switch from splitting to Russian roulette
    transparent(&tracer, 2, 200),                              // No. of splits before Russian roulette and max. trace depth
    volume(&tracer, 3),                                        // Trace depth at which to switch from splitting to Russian roulette
    mc_volume(&tracer, 1),                                     // Number of samples per pass in path tracing of scattering volumes
    mc_subsurf(&tracer, scene.get_lights(), 2000),             // Number of surface samples per pass in subsurface scattering
    radiance_transfer(&tracer, scene.get_lights(), 10, 10, 3), // No. of samples, no. of PRT samples, no. of SH bands
    tone_map(1.8)                                              // Gamma for gamma correction (2.2 is typical)
{ 
  shaders.push_back(&reflectance);                             // number key 0 (reflectance only)
  shaders.push_back(&lambertian);                              // number key 1 (direct lighting)
  shaders.push_back(&ambient);                                 // number key 2 (ambient occlusion)
  shaders.push_back(&mc_lambertian);                           // number key 3 (path tracing)
  shaders.push_back(&photon_caustics);                         // number key 4 (photon map caustics)
  shaders.push_back(&photon_lambertian);                       // number key 5 (photon mapping)
  shaders.push_back(&radiance_transfer);                       // number key 6 (precompute radiance transfer)
}

RenderEngine::~RenderEngine()
{
  delete vctrl;
}

void RenderEngine::load_files(int argc, char** argv)
{
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
      path_split.pop_back();
      string foldername = path_split.empty() ? "" : path_split.back(); 
      lower_case_string(filename);
      lower_case_string(foldername);
      Mat4x4f transform = identity_Mat4x4f();
      if(char_traits<char>::compare(filename.c_str(), "cornell", 7) == 0)
        transform = scaling_Mat4x4f(Vec3f(0.025f))*rotation_Mat4x4f(YAXIS, M_PIf);
      else if(char_traits<char>::compare(filename.c_str(), "bunny", 5) == 0)
        transform = translation_Mat4x4f(Vec3f(-3.0f, -0.85f, -8.0f))*scaling_Mat4x4f(Vec3f(25.0f));
      else if(char_traits<char>::compare(filename.c_str(), "justelephant", 12) == 0)
        transform = translation_Mat4x4f(Vec3f(-10.0f, 3.0f, -2.0f))*rotation_Mat4x4f(YAXIS, 0.5f);
      else if(char_traits<char>::compare(filename.c_str(), "glass_wine", 10) == 0
              || char_traits<char>::compare(filename.c_str(), "wine", 4) == 0)
        transform = scaling_Mat4x4f(Vec3f(5.0f))*rotation_Mat4x4f(XAXIS, -0.6f)*rotation_Mat4x4f(ZAXIS, 0.3f);
      else if(char_traits<char>::compare(filename.c_str(), "liquid_in_glass", 15) == 0)
        transform = translation_Mat4x4f(Vec3f(-4.0f, 0.0f, -4.0f));
      else if(char_traits<char>::compare(filename.c_str(), "cube", 4) == 0)
        transform = scaling_Mat4x4f(Vec3f(4.0f));
      else if(char_traits<char>::compare(filename.c_str(), "dragon", 6) == 0)
        transform = translation_Mat4x4f(Vec3f(0.0f, -0.05655f, 0.0f));
      else if(char_traits<char>::compare(filename.c_str(), "glass", 5) == 0)
        transform = scaling_Mat4x4f(Vec3f(100.0f));
      scene.load_mesh(argv[i], transform);
    }
  }
  else
  {
    cout << "Usage: pathtrace any_object.obj [another.obj ...]" << endl;
    exit(0);
  }
}

void RenderEngine::init_GLUT(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(winw, winh);
  glutCreateWindow("Path tracer");
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutMouseFunc(mouse);
  glutMotionFunc(move);
  glutIdleFunc(idle);
}

void RenderEngine::init_GL()
{
  glEnable(GL_CULL_FACE);
  //glEnable(GL_POINT_SMOOTH);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  // glew initialization
  GLenum err = glewInit();
  if(GLEW_OK != err)
  {
    cout << "Error: " << glewGetErrorString(err);
    exit(1);
  }
}

void RenderEngine::init_view()
{
  Vec3f c;
  float r;
  scene.get_bsphere(c, r);
  r *= 1.75f;

  // Initialize track ball
  vctrl = new GLViewController(winw, winh, c, r);

  // Initialize corresponding camera for tracer
  Vec3f eye, lookat, up;
	vctrl->get_view_param(eye, lookat, up);
  cam.set(eye, lookat, up, 1.0f);
}

void RenderEngine::init_tracer()
{
  // Insert background geometry and texture/color
  if(use_sun_and_sky)
    scene.add_plane(Vec3f(0.0f, 0.0f, 0.0f), world_up, "..\\models\\plane.mtl", 2); // standard plane
  
  tracer.set_background(background);
 
  if(!bgtex_filename.empty())
  {
    list<string> dot_split;
    Util::split(bgtex_filename, dot_split, ".");
    if(dot_split.back() == "hdr")
      bgtex.load_hdr(bgtex_filename.c_str());
    else
      bgtex.load(bgtex_filename.c_str());
    tracer.set_background(&bgtex);
  }

  if(use_sun_and_sky)
  {
    // Use the Julian date (day_of_year), the solar time (time_of_day), the latitude (latitude),
    // and the angle with South (angle_with_south) to find the direction toward the sun (sun_dir).
    //Vec3f sun_dir = Vec3f(-light_dir[0], light_dir[1], -light_dir[2]);
    // hard coded numbers are from Preetham et al.'s A Practical Analytical Model for Daylight, SIGGRAPH 1999
    float declination = 0.4093 * sin(2 * M_PIf * (day_of_year - 81) / 368);
    float theta = M_PIf * 0.5f - asin(sin(latitude) * sin(declination) - 
      cos(latitude) * cos(declination) * cos(M_PIf * time_of_day / 12));
    float phi = atan(-(cos(declination) * sin(M_PIf * time_of_day / 12)) / 
      (cos(latitude) * sin(declination) - sin(latitude) * cos(declination) * cos(M_PIf * time_of_day / 12)));

    sun_sky.setSunTheta(theta);
    sun_sky.setSunPhi(phi);
    sun_sky.setTurbidity(turbidity);
    sun_sky.init();
    tracer.set_background(&sun_sky);
  }

  // Set photon mapping parameters
  tracer.set_search_parameters(look_up_radius, caustics_photons_in_estimate, look_up_radius, global_photons_in_estimate);

  // Set shaders
  scene.set_shader(0, shaders[current_shader]); // shader for illum 0 (chosen by number key)
  scene.set_shader(1, shaders[current_shader]); // shader for illum 1 (chosen by number key)
  scene.set_shader(2, &mc_glossy);              // shader for illum 2
  scene.set_shader(3, &mirror);                 // shader for illum 3
  scene.set_shader(4, &transparent);            // shader for illum 4
  scene.set_shader(11, &metal);                 // shader for illum 11
  scene.set_shader(12, &volume);                // shader for illum 12
  scene.set_shader(13, &mc_volume);             // shader for illum 13
  scene.set_shader(14, &mc_subsurf);            // shader for illum 14
  
  // Add generated media and load media data
  Medium seawater = water(10.0, 35.0);
  seawater.name = "seawater";
  scene.set_medium(seawater);
  scene.set_medium(water());
  scene.set_medium(crown_glass());
  scene.set_medium(dense_flint_glass());
  scene.load_media("..\\models\\media.mpml");

    // Print out background color for the log
  cout << "Background ambient: " << background << endl;

  // Add polygons with an ambient material as area light sources
  unsigned int lights_in_scene = scene.extract_area_lights(&tracer, 4);  // Set number of samples per light source here
    
  if(use_sun_and_sky)
  {
    cout << "Adding sky light." << endl;
    scene.add_light(&sun_sky);
  }
  else if (use_default_light)
  {
    cout << "Adding default light: " << default_light.describe() << endl;
    Vec3f sun_dir = -light_dir;
    Vec3f solar_irrad = light_pow;
    default_light = Directional(&tracer, solar_irrad, -sun_dir);
    scene.add_light(&default_light);
  }

  // Identify volumes in scene for bidirectional path tracing and/or subsurface scattering
  volumes = scene.get_volumes();

  // Build acceleration data structure
  Timer timer;
  cout << "Building acceleration structure...";
  timer.start();
  scene.build_bsptree();
  timer.stop();
  cout << "(time: " << timer.get_time() << ")" << endl; 

  // Build photon maps
  cout << "Building photon maps..." << endl;
  timer.start();
  tracer.build_maps(caustics_particles, global_particles, max_to_trace);
  timer.stop();
  cout << "Building time: " << timer.get_time() << endl;
}

void RenderEngine::init_texture(const float* data, unsigned int& tex)
{
  if(!glIsTexture(tex))
    glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  // load the texture image
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, resx, resy, 0, GL_RGB, GL_FLOAT, data);
}

void RenderEngine::init_sample_to_volume()
{
  map<const TriMesh*, const Interface*>::const_iterator i = volumes.begin();
  for(; i != volumes.end(); ++i)
  {
    if(i->first->materials[i->first->mat_idx[0]].illum == 14)
      mc_subsurf.init_sample_surface(i->first, i->second);
  }
}

//////////////////////////////////////////////////////////////////////
// Rendering
//////////////////////////////////////////////////////////////////////

bool RenderEngine::toggle_final_gather()
{
  photon_lambertian.toggle_final_gathering();
  clear_image();
  return photon_lambertian.is_gathering();
}

void RenderEngine::clear_image()
{ 
  fill(&image[0], &image[0] + resx*resy, Vec3f(0.0f));
  sample_number = 0.0f;
  split_time = 0.0;
}

void RenderEngine::clear_sample_to_volume()
{
  map<const TriMesh*, const Interface*>::const_iterator i = volumes.begin();
  for(; i != volumes.end(); ++i)
    mc_subsurf.clear_surface_samples(i->first);
}

void RenderEngine::apply_tone_map()
{
  if(done)
  {
    tone_map.apply(image[0].get(), resx, resy, 3);
    init_texture(image[0].get(), image_tex);
    glutPostRedisplay();
  }
}

void RenderEngine::unapply_tone_map()
{
  if(done)
  {
    tone_map.unapply(image[0].get(), resx, resy, 3);
    init_texture(image[0].get(), image_tex);
    glutPostRedisplay();
  }
}

void RenderEngine::add_textures()
{
  reflectance.set_textures(scene.get_textures());
  lambertian.set_textures(scene.get_textures());
  ambient.set_textures(scene.get_textures());
  mc_lambertian.set_textures(scene.get_textures());
  photon_caustics.set_textures(scene.get_textures());
  tracer.set_use_textures_in_splat(true);
  scene.textures_on();
}

void RenderEngine::readjust_camera()
{
  Vec3f eye, lookat, up;
  vctrl->get_view_param(eye, lookat, up);
  cam.set(eye, lookat, up, cam.get_focal_dist());
}

void RenderEngine::raytrace()
{
  cout << "Raytracing";
  Timer timer;
  timer.start();

  init_sample_to_volume();
  #pragma omp parallel for private(randomizer)
  for(int j = 0; j < static_cast<int>(resy); ++j)
  {
    for(unsigned int i = 0; i < resx; ++i)
      image[i + j*resx] = tracer.compute_pixel(i, j);
    if(((j + 1) % 50) == 0) 
      cerr << ".";
  }

  timer.stop();
  cout << " - " << timer.get_time() << " secs " << endl;

  init_texture(image[0].get(), image_tex);
  done = true;
}

void RenderEngine::pathtrace()
{
  static Timer timer;
  static bool first = true;  
  if(first)
  {
    cout << "Render time:" << endl;
    first = false;
  } 
  cout << static_cast<int>(sample_number) + 1;
  timer.start(split_time);

  init_sample_to_volume();
  #pragma omp parallel for private(randomizer)
  for(int j = 0; j < static_cast<int>(resy); ++j)
  {
    for(unsigned int i = 0; i < resx; ++i)
      tracer.update_pixel(i, j, sample_number, image[i + j*resx]);
    if(((j + 1) % 50) == 0) 
      cerr << ".";
  }
  
  timer.stop();
  split_time = timer.get_time();
  cout << ": " << split_time << endl;
  ++sample_number;

  init_texture(image[0].get(), image_tex);
  display();
}

//////////////////////////////////////////////////////////////////////
// Export/import
//////////////////////////////////////////////////////////////////////

void RenderEngine::save_view(const string& filename) const
{
  float focal_dist = cam.get_focal_dist();
  ofstream ofs(filename.c_str(), ofstream::binary);
  if(ofs.bad())
    return;

  vctrl->save(ofs);
  ofs.write(reinterpret_cast<const char*>(&focal_dist), sizeof(float));
  ofs.close();
}

void RenderEngine::load_view(const string& filename)
{
  float focal_dist;
  ifstream ifs_view(filename.c_str(), ifstream::binary);
  if(ifs_view.bad())
    return;

  vctrl->load(ifs_view);
  bool found_fd = ifs_view.read(reinterpret_cast<char*>(&focal_dist), sizeof(float));
  ifs_view.close();
  
  Vec3f eye, lookat, up;
  vctrl->get_view_param(eye, lookat, up);
  cam.set(eye, lookat, up, found_fd ? focal_dist : cam.get_focal_dist());
}

void RenderEngine::export_render() const
{
  // export view
  save_view("result_view");

  // export render data
  ofstream ofs_data("result.txt");
  if(ofs_data.bad())
    return;
  ofs_data << sample_number << " " << split_time << endl << resx << " " << resy;
  ofs_data.close();

  // export image
  ofstream ofs_image("result.raw", ofstream::binary);
  if(ofs_image.bad())
    return;
  ofs_image.write(reinterpret_cast<const char*>(image[0].get()), resx*resy*sizeof(Vec3f));
  ofs_image.close();      
}

void RenderEngine::import_image(Vec3f* img) const
{
  ifstream ifs_image("result.raw", ifstream::binary);
  if(ifs_image.bad())
    return;
  ifs_image.read(reinterpret_cast<char*>(img[0].get()), resx*resy*sizeof(Vec3f));
  ifs_image.close();
}

void RenderEngine::import_render()
{
  // import view
  load_view("result_view");

  // import render data
  unsigned int old_resx, old_resy;
  ifstream ifs_data("result.txt");
  if(ifs_data.bad())
    return;
  ifs_data >> sample_number >> split_time >> old_resx >> old_resy;
  ifs_data.close();

  if(old_resx != resx || old_resy != resy)
  {
    cout << "Resolution mismatch between current render resolution and that of the render result to be imported." << endl;
    return;
  }

  // import image
  import_image(&image[0]);
  init_texture(image[0].get(), image_tex);
}

void RenderEngine::save_as_bitmap() const
{
  list<string> dot_split;
  Util::split(filename, dot_split, ".");
  string png_name = dot_split.front() + ".png";
  unsigned char* data = new unsigned char[resx*resy*3];
  #pragma omp parallel for
  for(int j = 0; j < static_cast<int>(resy); ++j)
    for(unsigned int i = 0; i < resx; ++i)
    {
      unsigned int d_idx = (i + resx*j)*3;
      unsigned int i_idx = i + resx*(resy - j - 1);
      for(unsigned int k = 0; k < 3; ++k)
        data[d_idx + k] = static_cast<unsigned int>(std::max(std::min(image[i_idx][k], 1.0f), 0.0f)*255.0f + 0.5f);
    }
  stbi_write_png(png_name.c_str(), resx, resy, 3, data, resx*3);
  delete [] data;
  cout << "Rendered image stored in " << png_name << "." << endl;
}

//////////////////////////////////////////////////////////////////////
// Error assessment
//////////////////////////////////////////////////////////////////////

void RenderEngine::measure_rmse() const
{
  Image ref_img;
  ref_img.data.resize(resx*resy);
  ref_img.width = resx;
  ref_img.height = resy;
  import_image(&ref_img.data[0]);
  ImageCompare icompare(ref_img);
  cout << "RMSE: " << sqrt(icompare.get_mse(Image(&image[0], resx, resy))) << endl;
  icompare.draw_difference(Image(&image[0], resx, resy));
  glutSwapBuffers();
}

void RenderEngine::measure_ssim() const
{
  Image ref_img;
  ref_img.data.resize(resx*resy);
  ref_img.width = resx;
  ref_img.height = resy;
  import_image(&ref_img.data[0]);
  ImageCompare icompare(ref_img);
  cout << "SSIM: " << icompare.get_ssim(Image(&image[0], resx, resy)) << endl;
}

//////////////////////////////////////////////////////////////////////
// Draw functions
//////////////////////////////////////////////////////////////////////

void RenderEngine::set_gl_ortho_proj() const
{
  glMatrixMode(GL_PROJECTION);	 
  glLoadIdentity();             
    
  glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);

  glMatrixMode(GL_MODELVIEW);  
}

void RenderEngine::angular_map_vertex(float x, float y) const
{
  static const float far_clip = 1.0e-6f - 1.0f;
  float u, v;
  Vec3f dir = normalize(cam.get_ray_dir(Vec2f(x - 0.5f, y - 0.5f)));
  bgtex.project_direction(dir, u, v);
  glColor3fv(tracer.get_background(dir).get());
  glTexCoord2f(u, v);
  glVertex3f(x, y, far_clip);
}

void RenderEngine::draw_angular_map_strip(const Vec4f& quad, float no_of_steps) const
{
  glBegin(GL_TRIANGLE_STRIP);
  float ystep = quad[3] - quad[1];
  for(float y = quad[1]; y < quad[3] + ystep; y += ystep/no_of_steps)
  {
    angular_map_vertex(quad[0], y);
    angular_map_vertex(quad[2], y);
  } 
  glEnd();
}

void RenderEngine::draw_angular_map_tquad(float no_of_xsteps, float no_of_ysteps) const
{
  float xstep = 1.0f/no_of_xsteps;
  for(float x = 0.0f; x < 1.0f + xstep; x += xstep)
    draw_angular_map_strip(Vec4f(x, 0.0f, x + xstep, 1.0f), no_of_ysteps);
}

void RenderEngine::draw_background() const
{
  if(!bgtex.has_texture() && !use_sun_and_sky)
    return;

  set_gl_ortho_proj();
  glLoadIdentity();

  if(bgtex.has_texture())
  {
    bgtex.bind();
    bgtex.enable();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  }
  draw_angular_map_tquad(10.0f, 10.0f);
  if(bgtex.has_texture())
    bgtex.disable();

  cam.glSetPerspective(winw, winh);
  cam.glSetCamera();
}

void RenderEngine::draw_texture() const
{
  static GLfloat verts[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };

  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

  glBindTexture(GL_TEXTURE_2D, image_tex);
  glEnable(GL_TEXTURE_2D);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  
  glVertexPointer(2, GL_FLOAT, 0, verts);
  glTexCoordPointer(2, GL_FLOAT, 0, verts);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);

  glDisable(GL_TEXTURE_2D);
}

void RenderEngine::draw()
{ 
  if(shaders[current_shader] == &photon_caustics)
    tracer.draw_caustics_map();
  else if(shaders[current_shader] == &photon_lambertian)
    tracer.draw_global_map();
  else
  {
    if(prt_file.is_open()
        && shaders[current_shader] == &radiance_transfer 
        && !scene.is_redoing_display_list())
    {
      radiance_transfer.set_prt_file(0);
      prt_file.close();
    }

    draw_background();
    scene.draw();
  }
}

//////////////////////////////////////////////////////////////////////
// GLUT callback functions
//////////////////////////////////////////////////////////////////////

void RenderEngine::display()
{
  static bool first = true;
  if(first)
  {
    first = false;
    glutTimerFunc(render_engine.get_spin_timer(), spin, 0);
    render_engine.init_sample_to_volume();
  }

  render_engine.readjust_camera();

  if(render_engine.is_done() || render_engine.is_tracing())
  {
    render_engine.set_gl_ortho_proj();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    render_engine.draw_texture();
  }
  else
  {
    glEnable(GL_DEPTH_TEST);
    render_engine.set_gl_perspective();
    render_engine.set_gl_clearcolor();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
	  render_engine.set_gl_camera();
    render_engine.draw();
    glDisable(GL_DEPTH_TEST);
  }
  glutSwapBuffers();  
}

void RenderEngine::reshape(int width, int height)
{
  render_engine.set_window_size(width, height);
  glViewport(0, 0, width, height);
}

void RenderEngine::keyboard(unsigned char key, int x, int y)
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
  //
  // When you switch shaders all previous rendering results
  // will be erased!
  if(key >= 48 && key < 48 + static_cast<unsigned char>(render_engine.no_of_shaders()))
  {
    unsigned int shader_no = key - 48;
    if(shader_no != render_engine.get_current_shader())
    {
      render_engine.set_current_shader(shader_no);
      render_engine.clear_image();
      render_engine.clear_sample_to_volume();
      render_engine.redo_display_list();
      cout << "Switched to shader number " << shader_no << endl;
      glutPostRedisplay();
    }
  }
  switch(key)
  { 
  // Use '+' and '-' to increase or decrease the number of
  // jitter samples per pixel in a simple ray tracing
  case '+':
    render_engine.increment_pixel_subdivs();
    break;
  case '-':
    render_engine.decrement_pixel_subdivs();
    break;
  // Press '*' to apply tone mapping
  case '*':
    render_engine.apply_tone_map();
    break;
  // Press '/' to unapply tone mapping
  case '/':
    render_engine.unapply_tone_map();
    break;
  // Press 'b' to save the render result as a bitmap called out.png.
  // If obj files are loaded, the png will be named after the obj file loaded last.
  case 'b':
    render_engine.save_as_bitmap();
    break;
  // Press 'c' to print the camera properties
  case 'c':
    {
      Vec3f eye, lookat, up;
      render_engine.get_view_controller()->get_view_param(eye, lookat, up);
      cout << "Eye   : " << eye << endl
	         << "Lookat: " << lookat << endl
	         << "Up    : " << up << endl
           << "FOV   : " << render_engine.get_focal_dist() << endl;
    } 
    break;
  // Press 'e' to export the current rendering result to "result.raw", "result.txt", and "result_view".
  // Use 'i' to import later and continue rendering.
  case 'e':
    cout << "Exporting render result to files: result.raw, result.txt, result_view" << endl;
    render_engine.export_render();
    break;
  // Press 'g' to toggle final gathering on/off in photon mapping.
  case 'g':
    {
      bool is_gathering = render_engine.toggle_final_gather();
      cout << "Toggled final gathering " << (is_gathering ? "on" : "off") << endl;
    }
    break;    
  // Press 'i' to import a previous rendering result from "result.raw", "result.txt", and "result_view".
  // After import you can continue rendering (if the same geometry has been loaded).
  case 'i':
    cout << "Importing render result from files: result.raw, result.txt, result_view" << endl;
    render_engine.import_render();
    glutPostRedisplay();
    break;
  // Press 'm' to get the mean square error of the current rendering as compared to "result.raw"
  case 'm':
    render_engine.measure_rmse();
    break;
  // Press 'r' to start a simple ray tracing (one pass -> done).
  // To switch back to preview mode after the ray tracing is done
  // press 'r' again.
  case 'r':
    if(render_engine.is_done())
    {
      render_engine.undo();
      glutPostRedisplay();
    }
    else 
      render_engine.start_trace();
    break;
  // Press 's' to toggle shadows on/off
  case 's':
    {
      bool shadows_on = render_engine.toggle_shadows();
      render_engine.clear_image();
      render_engine.redo_display_list();
      cout << "Toggled shadows " << (shadows_on ? "on" : "off") << endl;
      glutPostRedisplay();
    }
    break;
  // Press 't' to start path tracing. The path tracer will
  // continue to improve the image until 't' is pressed again.
	case 't':
    {
	    bool is_tracing = render_engine.toggle_pathtracing();
	    if(!is_tracing && !render_engine.is_done())
        render_engine.undo();
    }
	  break;
  // Press 'x' to switch on material textures.
  case 'x':
    render_engine.add_textures();
    cout << "Toggled textures on." << endl;
    glutPostRedisplay();
    break;
  // Press 'z' to zoom in.
  case 'z':
    {
      render_engine.set_focal_dist(render_engine.get_focal_dist()*1.05f);
      glutPostRedisplay();
    }
    break;
  // Press 'L' to load a view saved in the file "view".
  case 'L':
    {
      render_engine.load_view("view");
      cout << "View loaded from file: view" << endl;
      Vec3f eye, lookat, up;
      render_engine.get_view_controller()->get_view_param(eye, lookat, up);
      cout << "Eye   : " << eye << endl
	         << "Lookat: " << lookat << endl
	         << "Up    : " << up << endl
           << "FOV   : " << render_engine.get_field_of_view() << endl;
      glutPostRedisplay();
    }
    break;
  // Press 'M' to get the SSIM of the current rendering as compared to "result.raw"
  case 'M':
    render_engine.measure_ssim();
    break;
  // Press 'S' to save the current view in the file "view".
  case 'S':
    render_engine.save_view("view");
    cout << "Current view stored in file: view" << endl;
    break;
  // Press 'Z' to zoom out.
  case 'Z':
    {
      render_engine.set_focal_dist(render_engine.get_focal_dist()/1.05f);
      glutPostRedisplay();
    }
    break;
  // Press 'space' to switch between pre-view and your last tracing result.
  case 32:
    render_engine.undo();
    if(!render_engine.is_done())
      glutTimerFunc(render_engine.get_spin_timer(), spin, 0); 
    glutPostRedisplay();
    break;
  // Press 'esc' to exit the program.
  case 27:
    exit(0);
  }
}

void RenderEngine::mouse(int btn, int state, int x, int y)
{
  if(state == GLUT_DOWN) 
  {
    if(btn == GLUT_LEFT_BUTTON) 
      render_engine.get_view_controller()->grab_ball(ROTATE_ACTION, Vec2i(x, y));
    else if(btn == GLUT_MIDDLE_BUTTON) 
      render_engine.get_view_controller()->grab_ball(ZOOM_ACTION, Vec2i(x, y));
    else if(btn == GLUT_RIGHT_BUTTON) 
      render_engine.get_view_controller()->grab_ball(PAN_ACTION, Vec2i(x, y));
  }
  else if(state == GLUT_UP)
			render_engine.get_view_controller()->release_ball();

  render_engine.set_mouse_state(state);
}

void RenderEngine::move(int x, int y)
{
  render_engine.get_view_controller()->roll_ball(Vec2i(x, y));
  if(render_engine.get_mouse_state() == GLUT_DOWN)
    glutPostRedisplay();
}

void RenderEngine::spin(int x)
{
  if(!render_engine.is_tracing())
  {
    if(render_engine.get_view_controller()->try_spin())
      glutPostRedisplay();
    glutTimerFunc(render_engine.get_spin_timer(), spin, 0);  
  }
}

void RenderEngine::idle()
{ 
  if(render_engine.is_starting_trace())
  {
    render_engine.end_trace();
    render_engine.raytrace();
    glutPostRedisplay();
  }
  else if(render_engine.is_tracing())
    render_engine.pathtrace();
}

//////////////////////////////////////////////////////////////////////
// Accessors
//////////////////////////////////////////////////////////////////////

void RenderEngine::set_current_shader(unsigned int shader) 
{ 
  current_shader = shader;
  for(int i = 0; i < 2; ++i)
    scene.set_shader(i, shaders[current_shader]);
  if(shaders[current_shader] != &photon_caustics && shaders[current_shader] != &photon_lambertian)
    scene.redo_display_list();
  if(shaders[current_shader] == &radiance_transfer)
  {
    prt_file.open(prt_filename.c_str(), ios_base::out | ios_base::binary);
    radiance_transfer.set_prt_file(&prt_file);
  }
}
