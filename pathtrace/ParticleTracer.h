// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2010
// Copyright (c) DTU Informatics 2010

#ifndef PARTICLE_TRACER
#define PARTICLE_TRACER

#include "CGLA/Vec3f.h"
#include "Ray.h"
#include "ObjMaterial.h"
#include "../optprops/Medium.h"
#include "PhotonMap.h"
#include "Light.h"
#include "Texture.h"
#include "PathTracer.h"

class ParticleTracer : public PathTracer
{
public:
  ParticleTracer(unsigned int w, 
                 unsigned int h, 
                 Scene* s, 
                 unsigned int max_no_of_particles,
                 unsigned int pixel_subdivs = 1);

  void build_maps(int no_of_caustic_particles, 
                  int no_of_global_particles, 
                  unsigned int max_no_of_shots = 500000);
  void draw_caustics_map();
  void draw_global_map();

  virtual CGLA::Vec3f compute_pixel(unsigned int x, unsigned int y) const;
	virtual void update_pixel(unsigned int x, unsigned int y, float sample_number, CGLA::Vec3f& L) const;

  CGLA::Vec3f caustics_irradiance(const Ray& r, float max_distance, int no_of_particles) const;
  CGLA::Vec3f global_irradiance(const Ray& r, float max_distance, int no_of_particles) const;

  // Functions for computing direct visualization of photon map as an overlay
  void set_search_parameters(float caustics_max_dist, int caustics_nparticles, float global_max_dist, int global_nparticles);
  void splat_caustics_map() { splat_map(true); }
  void splat_global_map() { splat_map(false); }
  void redo_G_buffer() { redraw = true; }
  void release_render_texture() { render_tex.clear(); }
  void set_use_textures_in_splat(bool use_textures_in_splat) { use_textures = use_textures_in_splat; } 

protected:
  void trace_particle(const Light* light, const unsigned int caustics_done, const unsigned int global_done);
  bool disperse_particle(Ray& r, const ObjMaterial*& m, int& color_band, CGLA::Vec3f& Phi) const;
  CGLA::Vec3f get_diffuse(const Ray& r) const;
  CGLA::Vec3f get_transmittance(const Ray& r, const Medium* m) const;
  void splat_map(bool caustic);
  void draw_render_tex() const;

  PhotonMap<> caustics;
  PhotonMap<> global;
  Color<double> spectrum_cdf;
  Color<CGLA::Vec3f> normalized_CIE_rgb;
  Texture render_tex;
  bool redraw;
  bool use_textures;
  float c_max_dist;
  int c_np;
  float g_max_dist;
  int g_np;
};

#endif // PARTICLE_TRACER