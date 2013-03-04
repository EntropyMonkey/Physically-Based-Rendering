// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2013
// Copyright (c) DTU Compute 2013

#ifndef RENDERENGINE_H
#define RENDERENGINE_H

#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <GL/glut.h>
#include "CGLA/Vec3f.h"
#include "GLGraphics/GLViewController.h"
#include "Camera.h"
#include "Scene.h"
#include "Directional.h"
#include "SunSky.h"
#include "SphereTexture.h"
#include "ParticleTracer.h"
#include "Shader.h"
#include "Textured.h"
#include "Lambertian.h"
#include "Glossy.h"
#include "Ambient.h"
#include "MCLambertian.h"
#include "MCGlossy.h"
#include "PhotonCaustics.h"
#include "PhotonLambertian.h"
#include "Mirror.h"
#include "Metal.h"
#include "Transparent.h"
#include "Volume.h"
#include "MCVolume.h"
#include "MCSubsurf.h"
#include "RadianceTransfer.h"
#include "Gamma.h"

class RenderEngine
{
public:
  // Initialization
  RenderEngine();
  ~RenderEngine();
  void load_files(int argc, char** argv);
  void init_GLUT(int argc, char** argv);
  void init_GL();
  void init_view();
  void init_tracer();
  void init_texture(const float* data, unsigned int& tex);
  void init_sample_to_volume();

  // Rendering
  unsigned int no_of_shaders() const { return shaders.size(); }
  bool is_starting_trace() const { return start_raytrace; }
  bool is_tracing() const { return tracing; }
  bool is_done() const { return done; }
  void start_trace() { start_raytrace = true; }
  void end_trace() { start_raytrace = false; }
  void undo() { done = !done; }
  void increment_pixel_subdivs() { tracer.increment_pixel_subdivs(); }
  void decrement_pixel_subdivs() { tracer.decrement_pixel_subdivs(); }
  bool toggle_pathtracing() { return tracing = !tracing; }
  bool toggle_shadows() { shadows_on = !shadows_on; scene.toggle_shadows(); return shadows_on; }
  bool toggle_final_gather();
  void clear_image();
  void clear_sample_to_volume();
  void apply_tone_map();
  void unapply_tone_map();
  void add_textures();
  void readjust_camera();
  void raytrace();
  void pathtrace();

  // Export/import
  void save_view(const std::string& filename) const;
  void load_view(const std::string& filename);
  void export_render() const;
  void import_render();
  void import_image(CGLA::Vec3f* img) const;
  void save_as_bitmap() const;

  // Error assessment
  void measure_rmse() const;
  void measure_ssim() const;

  // Draw functions
  void set_gl_ortho_proj() const;
  void set_gl_perspective() const { cam.glSetPerspective(winw, winh); }
  void set_gl_camera() const { cam.glSetCamera(); }
  void set_gl_clearcolor() const { glClearColor(background[0], background[1], background[2], 0.0f); }
  void redo_display_list() { scene.redo_display_list(); }
  void angular_map_vertex(float x, float y) const;
  void draw_angular_map_strip(const CGLA::Vec4f& quad, float no_of_steps) const;
  void draw_angular_map_tquad(float no_of_xsteps, float no_of_ysteps) const;
  void draw_background() const;
  void draw_texture() const;
  void draw();

  // GLUT callback functions
  static void display();
  static void reshape(int width, int height);
  static void keyboard(unsigned char key, int x, int y);
  static void mouse(int btn, int state, int x, int y);
  static void move(int x, int y);
  static void spin(int x);
  static void idle();

  // Accessors
  void set_window_size(int w, int h) { winw = w; winh = h; vctrl->reshape(w, h);  }
  unsigned int get_current_shader() const { return current_shader; }
  void set_current_shader(unsigned int shader);
  float get_focal_dist() const { return cam.get_focal_dist(); }
  void set_focal_dist(float fd) { cam.set_focal_dist(fd); }
  GLGraphics::GLViewController* get_view_controller() const { return vctrl; }
  float get_field_of_view() const { return cam.get_fov(); }
  int get_mouse_state() const { return mouse_state; }
  void set_mouse_state(int state) { mouse_state = state; }
  int get_spin_timer() const { return spin_timer; }

private:
  // Window and render resolution
  unsigned int winw, winh;
  unsigned int resx, resy;

  // Render data
  std::vector<CGLA::Vec3f> image;
  unsigned int image_tex;
  float sample_number;
  double split_time;

  // View control
  int mouse_state;
  int spin_timer;
  GLGraphics::GLViewController* vctrl;
  Camera cam;

  // Geometry container
  Scene scene;
  
  // Output file name
  std::string filename;

  // Tracer
  ParticleTracer tracer;
  unsigned int max_to_trace;
  unsigned int caustics_particles;
  unsigned int global_particles;
  unsigned int caustics_photons_in_estimate;
  unsigned int global_photons_in_estimate;
  float look_up_radius;

  // Tracing state
  bool start_raytrace;
  bool tracing;
  bool done;
  bool shadows_on;

  // Light
  bool use_default_light;
  CGLA::Vec3f light_pow;
  CGLA::Vec3f light_dir;
  Directional default_light;

  // Environment
  bool use_sun_and_sky;
  CGLA::Vec3f background;
  SphereTexture bgtex;
  std::string bgtex_filename;
  double day_of_year;
  double time_of_day;
  double latitude;
  CGLA::Vec3f world_up;
  double angle_with_south;
  double turbidity;
  PreethamSunSky sun_sky;

  // References to volumes for bidirectional path tracing
  std::map<const TriMesh*, const Interface*> volumes;

  // Output file for precomputed radiance transfer
  std::string prt_filename;
  std::ofstream prt_file;

  // Shaders
  unsigned int current_shader;
  std::vector<Shader*> shaders;
  Textured reflectance;
  Lambertian lambertian;
  Glossy glossy;
  Ambient ambient;
  MCLambertian mc_lambertian;
  MCGlossy mc_glossy;
  PhotonCaustics photon_caustics;
  PhotonLambertian photon_lambertian;
  Mirror mirror;
  Metal metal;
  Transparent transparent;
  Volume volume;
  MCVolume mc_volume;
  MCSubsurf mc_subsurf;
  RadianceTransfer radiance_transfer;

  // Tone mapping
  Gamma tone_map;
};

extern RenderEngine render_engine;

#endif // RENDERENGINE