// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2013
// Copyright (c) DTU Compute 2013

#include <iostream>
#include <vector>
#include <complex>
#include "GL/glew.h"
#include "GL/glut.h"
#include "CGLA/Vec3f.h"
#include "../optprops/Medium.h"
#include "../optprops/spectrum2rgb.h"
#include "Ray.h"
#include "ObjMaterial.h"
#include "mt_random.h"
#include "sampler.h"
#include "cdf_bsearch.h"
#include "luminance.h"
#include "Timer.h"
#include "Shader.h"
#include "PhotonCaustics.h"
#include "PhotonLambertian.h"
#include "ParticleTracer.h"

#ifdef _OPENMP
  #include <omp.h>
#endif

using namespace std;
using namespace CGLA;

namespace
{
  void draw_texture(const Texture& tex, float width, float height)
  {
    static GLfloat verts[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };
    GLfloat texcoords[] = { 0.0f, 0.0f, width, 0.0f, 0.0f, height, width, height };

    glMatrixMode(GL_PROJECTION);	 
    glLoadIdentity();             
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    tex.bind();
    tex.enable();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    glVertexPointer(2, GL_FLOAT, 0, verts);
    glTexCoordPointer(2, GL_FLOAT, 0, texcoords);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    tex.disable();
  }
}

ParticleTracer::ParticleTracer(unsigned int w, 
                               unsigned int h, 
                               Scene* s, 
                               unsigned int max_no_of_particles,
                               unsigned int pixel_subdivs)
  : PathTracer(w, h, s, pixel_subdivs), 
    caustics(max_no_of_particles), global(max_no_of_particles),
    spectrum_cdf(spectrum_rgb_samples), normalized_CIE_rgb(spectrum_rgb_samples),
    redraw(true), use_textures(false)
{ 
  Vec3f cie_rgb(0.0f);
  double cie = 0.0;
  for(unsigned int i = 0; i < spectrum_rgb_samples; ++i)
  {
    for(unsigned int j = 1; j < 4; ++j)
    {
      double rgb = spectrum_rgb[i][j];
      normalized_CIE_rgb[i][j - 1] = rgb;
      cie += rgb;
    }
    cie_rgb += normalized_CIE_rgb[i];
    spectrum_cdf[i] = cie;
  }
  double wavelength = spectrum_rgb[0][0];
  double step_size = spectrum_rgb[1][0] - spectrum_rgb[0][0];
  spectrum_cdf /= cie;
  spectrum_cdf.wavelength = wavelength;
  spectrum_cdf.step_size = step_size;
  normalized_CIE_rgb /= cie_rgb;
  normalized_CIE_rgb.wavelength = wavelength;
  normalized_CIE_rgb.step_size = step_size;
}

void ParticleTracer::build_maps(int no_of_caustic_particles, 
                                int no_of_global_particles,
                                unsigned int max_no_of_shots)
{
  // Retrieve light sources
  const vector<Light*>& lights = scene->get_lights();
  if(lights.size() == 0)
    return;

  // Check requested photon counts
  if(no_of_caustic_particles > caustics.get_max_photon_count())
  {
    cerr << "Requested no. of caustic particles exceeds the maximum no. of particles." << endl;
    no_of_caustic_particles = caustics.get_max_photon_count();
  }
  if(no_of_global_particles > global.get_max_photon_count())
  {
    cerr << "Requested no. of global particles exceeds the maximum no. of particles." << endl;
    no_of_global_particles = global.get_max_photon_count();
  }

  // Choose block size
  int block = std::max(1, std::max(no_of_caustic_particles, no_of_global_particles)/100);

  // Shoot particles
  unsigned int nshots = 0;
  unsigned int caustics_done = no_of_caustic_particles == 0 ? 1 : 0;
  unsigned int global_done = no_of_global_particles == 0 ? 1 : 0;
  while(!caustics_done || !global_done)
  {
    // Stop if we cannot find the desired number of photons.
    if(nshots >= max_no_of_shots)
    {
      cerr << "Unable to store enough particles." << endl;
      if(!caustics_done)
        caustics_done = nshots;
      if(!global_done)
        global_done = nshots;
      break;
    }
    
    // Trace a block of photons at the time
    #pragma omp parallel for private(randomizer)
    for(int i = 0; i < block; ++i)
    {
      // Sample a light source
      unsigned int light_idx = static_cast<unsigned int>(lights.size()*mt_random());
      while(light_idx == lights.size())
        light_idx = static_cast<unsigned int>(lights.size()*mt_random());

      // Shoot a particle from the sampled source
      trace_particle(lights[light_idx], caustics_done, global_done);
    }
    nshots += block;

    // Check particle counts
    if(!caustics_done && caustics.get_photon_count() >= no_of_caustic_particles)
      caustics_done = nshots;
    if(!global_done && global.get_photon_count() >= no_of_global_particles)
      global_done = nshots;
  }
  cout << "Particles in caustics map: " << caustics.get_photon_count() << endl
       << "Particles in global map: " << global.get_photon_count() << endl;

  // Finalize photon maps
  caustics.scale_photon_power(lights.size()/static_cast<float>(caustics_done));
  global.scale_photon_power(lights.size()/static_cast<float>(global_done));
  caustics.balance();
  global.balance();
}

Vec3f ParticleTracer::compute_pixel(unsigned int x, unsigned int y) const
{
  Vec3f result(0.0);
  Vec2f vp_pos = Vec2f(x, y)*win_to_vp + lower_left;

  for(unsigned int i = 0; i < jitter.size(); ++i)
  {
    Ray r = scene->get_camera()->get_ray(vp_pos + jitter[i]);
    if(scene->intersect(r))
    {
      const Shader* s = get_shader(r);
      if(s)
        result += s->shade(r);
      if(render_tex.has_texture())
      {
        Vec2f pixel_pos = vp_pos - lower_left;
        result += render_tex.sample_nearest(pixel_pos[0], 1.0f - pixel_pos[1] - win_to_vp[1]);
      }
    }
    else
      result += get_background(r.direction);
  }
  return result/static_cast<float>(jitter.size());
}

void ParticleTracer::update_pixel(unsigned int x, unsigned int y, float sample_number, Vec3f& L) const
{
  Vec2f vp_pos = Vec2f(x + mt_random(), y + mt_random())*win_to_vp;
  Ray r = scene->get_camera()->get_ray(vp_pos + lower_left);

  L *= sample_number;
  if(trace(r))
  {
    const Shader* s = get_shader(r);
    if(s)
      L += s->shade(r);
    if(render_tex.has_texture())
      L += render_tex.sample_nearest(vp_pos[0], 1.0f - vp_pos[1] - win_to_vp[1]);
  }
  else
    L += get_background(r.direction);
	L /= sample_number + 1.0f;
}

Vec3f ParticleTracer::caustics_irradiance(const Ray& r, float max_distance, int no_of_particles) const
{
  return caustics.irradiance_estimate(r.hit_pos, r.hit_normal, max_distance, no_of_particles);
}

Vec3f ParticleTracer::global_irradiance(const Ray& r, float max_distance, int no_of_particles) const
{
  return global.irradiance_estimate(r.hit_pos, r.hit_normal, max_distance, no_of_particles);  
}

void ParticleTracer::draw_caustics_map()
{
  caustics.draw();
}

void ParticleTracer::draw_global_map()
{
  global.draw();
}

void ParticleTracer::trace_particle(const Light* light, const unsigned int caustics_done, const unsigned int global_done)
{
  // Shoot a particle from the sampled source
  Ray r;
  Vec3f Phi;
  if(!light->emit(r, Phi))
    return;

  // Check if this particle should be added to the caustics map
  const ObjMaterial* m = r.get_hit_material();
  bool is_caustic_particle = !caustics_done && scene->is_specular(m);

  // Start tracing the path
  while(true)
  {
    // Forward from all specular surfaces
    int idx = -1;
    while(scene->is_specular(m) && r.trace_depth < 500)
    {
      switch(m->illum)
      {
      case 3:
        {
          Ray reflected;
          if(!trace_reflected(r, reflected))
            return;
          r = reflected;
          m = reflected.get_hit_material();
        }
        break;
      case 4:
      case 12:
        {
          double R;
          Ray refracted;
          trace_refracted(r, refracted, R);
          if(mt_random() < R)
          {
            Ray reflected;
            if(!trace_reflected(r, reflected))
              return;
            r = reflected;
            m = reflected.get_hit_material();
          }
          else
          {
            if(!refracted.has_hit)
              return;
            refracted.inside = dot(refracted.direction, refracted.hit_normal) > 0.0f;
            r = refracted;
            m = refracted.get_hit_material();
            if(m->illum > 11 && r.inside)
            {
              const Medium* med = scene->get_interface(m->name).med_in;
              Vec3f Tr = get_transmittance(r, med);
              double transmittance = get_luminance_NTSC(Tr[0], Tr[1], Tr[2]);
              if(mt_random() < transmittance)
                Phi *= Tr/transmittance;
              else
                return;
            }
          }
        }
        break;
      case 5:
        if(!disperse_particle(r, m, idx, Phi))
          return;
        break;
      case 11:
        {
          Vec3f R;
          Ray reflected;
          if(!trace_reflected(r, reflected, R))
            return;
          double prob = get_luminance_Avg(R[0], R[1], R[2]);
          if(mt_random() < prob)
          {
            Phi *= R/prob;
            r = reflected;
            m = reflected.get_hit_material();
          }
          else
            return;
        }
        break;
      default: 
        return;
      }
    }

    Phi /= r.ior*r.ior; // Take compression of solid angle into account
    Vec3f rho_d = get_diffuse(r);
    double prob = get_luminance_NTSC(rho_d[0], rho_d[1], rho_d[2]);

    // Store in caustics map at first diffuse surface
    if(is_caustic_particle)
    {
      if(prob > 0.0)
        #pragma omp critical
        caustics.store(Phi, r.hit_pos, -r.direction);
      is_caustic_particle = false;
    }
    if(global_done)
      return;

    // Store particle in global map and check limit
    #pragma omp critical
    global.store(Phi, r.hit_pos, -r.direction);

    // Continue tracing if not absorbed
    if(mt_random() < prob)
    {
      r = Ray(r.hit_pos, sample_cosine_weighted(r.hit_normal));
      if(!trace(r))
        return;
      Phi *= rho_d/prob;
      m = r.get_hit_material();
    }
    else
      return;
  }
}

bool ParticleTracer::disperse_particle(Ray& r, const ObjMaterial*& m, int& idx, Vec3f& Phi) const
{
  // Implement dispersion using Russian roulette.
  //
  // Input:  r   (the ray carrying the particle)
  //         m   (the material that the particle was traced from)
  //         idx (the wavelength index of a monochrometic particle, -1 if not monochromatic)
  //         Phi (the flux carried by the particle)
  //
  // Output: r   (the reflected or refracted ray carrying the particle)
  //         m   (the material that the particle hit)
  //         idx (the wavelength index of a monochrometic particle, -1 if not monochromatic)
  //         Phi (the flux carried by the particle after reflection or dispersion)
  //
  // Return: true if the reflected or refracted ray hit something, false otherwise.
  //
  // Relevant data fields that are available (see ParticleTracer.h):
  // spectrum_cdf       (importance weighted cumulative distribution function for spectral dispersion)
  // normalized_CIE_rgb (normalized CIE rgb colour matching functions for spectrum to RGB translation)
  // ior.wavelength     (wavelength corresponding to first IOR in array)
  // ior.step_size      (spectral distance to between wavelengths in IOR array)
  //
  // Hint: (a) Use the functions trace_reflected(...) and trace_dispersive(...)
  //       to trace a new ray in the reflected or the refracted direction. Retrieve the
  //       Fresnel reflectance by supplying a third argument to one of these functions.
  //       (b) Use the function cdf_bsearch(...) to get the index of the element in 
  //       spectrum_cdf that corresponds to a sampled random number in [0,1].
  //       (c) Transform between index i and wavelength using 
  //                       wavelength = xxx.wavelength + i*xxx.step_size ,
  //       where xxx is spectrum_cdf, for example.

  // If the ray is not monochromatic (idx < 0), sample a monochromatic ray.
  if(idx < 0)
  { 
    // Retrieve index of refraction (rgb or spectrum, see last argument)
    const Color< complex<double> >& ior = scene->get_medium(m).get_ior(rgb);
    if(ior.size() == 0)
      return false;

    // Sample a wavelength and set idx and Phi

  }

  // Sample reflection or refraction using Russian roulette and set r and m. 
  // Return true if something was hit.
  return false;
}

Vec3f ParticleTracer::get_diffuse(const Ray& r) const
{
  const ObjMaterial* m = r.get_hit_material();
  return m ? Vec3f(m->diffuse[0], m->diffuse[1], m->diffuse[2]) : Vec3f(0.8f);
}

// Transforming 1/m -> 1/cm
Vec3f ParticleTracer::get_transmittance(const Ray& r, const Medium* m) const
{
  if(m)
  {
    const Color<double>& ext = m->get_extinction(rgb);
    return Vec3f(exp(-ext[0]*1.0e-2*r.dist), exp(-ext[1]*1.0e-2*r.dist), exp(-ext[2]*1.0e-2*r.dist));
  }
  return Vec3f(1.0f);
}

void ParticleTracer::set_search_parameters(float caustics_max_dist, int caustics_nparticles, float global_max_dist, int global_nparticles)
{
  c_max_dist = caustics_max_dist;
  c_np = caustics_nparticles;
  g_max_dist = global_max_dist;
  g_np = global_nparticles;
}

void ParticleTracer::splat_map(bool caustic)
{
  if(redraw)
  {    
    redraw = false;
    Timer t;
    t.start();

    // switch off background color
    Vec3f bg_color = background;
    background = Vec3f(0.0f);

    // render direct visualization of map
    vector<Vec3f> render(width*height);
    PhotonCaustics photon_caustics(this, scene->get_lights(), c_max_dist, c_np);
    PhotonLambertian photon_lambertian(this, scene->get_lights(), g_max_dist, g_np, true, 3, &photon_caustics);
    Shader* photon_shader = caustic ? &photon_caustics : &photon_lambertian;
    vector<Shader*> shaders = scene->get_shaders();
    if(use_textures)
    {
      photon_caustics.set_textures(scene->get_textures());
      photon_lambertian.set_textures(scene->get_textures());
    }
    scene->set_shader(0, photon_shader);
    scene->set_shader(1, photon_shader);
    scene->set_shader(13, 0);
    scene->set_shader(14, 0);
    scene->set_shader(17, 0);
    #pragma omp parallel for private(randomizer)
    for(int j = 0; j < static_cast<int>(height); ++j)
    {
      for(unsigned int i = 0; i < width; ++i)
        render[i + j*width] = compute_pixel(i, j);
    }

    // Store in texture
    GLuint result_tex = 0;
    glGenTextures(1, &result_tex);
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, result_tex);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB32F_ARB, width, height, 0, GL_RGB, GL_FLOAT, render[0].get());
    render_tex.load(GL_TEXTURE_RECTANGLE_ARB, result_tex);

    // switch back to original shaders and background color
    for(unsigned int i = 0; i < shaders.size(); ++i)
      scene->set_shader(i, shaders[i]);
    background = bg_color;

    t.stop();
    cout << "Time for radiance estimates: " << t.get_time() << endl;
  }
  draw_render_tex();
}

void ParticleTracer::draw_render_tex() const
{
  draw_texture(render_tex, width, height);
}
