// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2013
// Copyright (c) DTU Compute 2013

#include <cmath>
#include <complex>
#include "CGLA/Vec3f.h"
#include "../optprops/Interface.h"
#include "../optprops/Medium.h"
#include "Ray.h"
#include "mt_random.h"
#include "sampler.h"
#include "luminance.h"
#include "fresnel.h"
#include "cdf_bsearch.h"
#include "MCSubsurf.h"

using namespace std;
using namespace CGLA;

namespace
{
  const double M_4PI = 4.0*M_PI;
  const Vec3f one(1.0f);
}

Vec3f MCSubsurf::shade(Ray& r, bool emit) const
{
  Vec3f result = MCVolume::shade(r, emit);
  const Medium* new_m = MCVolume::tracer->get_hit_medium(r, r.inside); // This is the material of the refracted ray
  if(new_m && new_m->turbid)
  {
    // Compute diffusion properties of the hit medium
    DiffusionProperties dp(new_m->get_ior(rgb), MCVolume::tracer->get_medium(r)->get_ior(rgb));
    if(!compute_diffusion_properties(r, new_m, &dp))
      return result;

    // Use exponential term to do a Russian roulette that accepts closer samples more often
    const vector<PositionSample>& sample_vec = pos_samples.find(r.hit_object)->second;
    double mean_transport = get_luminance_Avg(dp.transport[0], dp.transport[1], dp.transport[2]);

    // Compute contributions from position samples
    Vec3f diffusion_result(0.0f);
    for(unsigned int i = 0; i < sample_vec.size(); ++i)
    {
      const PositionSample& sample = sample_vec[i];
      if(sample.initialized)
      {
        double dist = length(sample.pos - r.hit_pos);
        double prob = exp(-dist*mean_transport);
        if(mt_random() < prob)
          diffusion_result += diffusion(r, new_m, dp, sample)/prob;
      }
    }
    diffusion_result /= static_cast<float>(sample_vec.size());
    result += diffusion_result;
    r.did_hit_diffuse = true;
  }
  return result;
}

void MCSubsurf::init_sample_surface(const TriMesh* mesh, const Interface* m)
{
  std::vector<PositionSample>& psamples = pos_samples[mesh];
  if(psamples.size() != samples)
	  psamples.resize(samples);

  // Get geometry info
  const IndexedFaceSet& geometry = mesh->geometry;
	const IndexedFaceSet& normals = mesh->normals;
	const float no_of_faces = geometry.no_faces();

  for(unsigned int i = 0; i < samples; ++i)
  {
    PositionSample& sample = psamples[i];
    sample.initialized = false;

    // Sample a triangle face

    // Sample Barycentric coordinates (set sample.u and sample.v)

    // Compute hit position (set sample.pos)

    // Compute hit normal (set sample.normal)

    // Trace a sample ray from the sampled position to sample the incident radiance

    // Find the cosine term where light is incident on the surface (cos_theta_i)

    // Find the direction of the transmitted ray (cos_theta_t)
    double ior1 = (m->med_out ? m->med_out->get_ior(mono)[0].real() : 1.0);
    double ior2 = m->med_in->get_ior(mono)[0].real();

    // Compute Frensel transmittance and handle total internal reflection
    const Color< complex<double> >& ior_i = m->med_in->get_ior(rgb);
    Color< complex<double> > ior_o; 
    if(m->med_out)
      ior_o = m->med_out->get_ior(rgb);
    else
      ior_o.resize(3, complex<double>(1.0, 0.0));

    // Compute radiance entering the medium at the sampled position (set sample.L)
    sample.L = Vec3f(0.0f);
    if(dot(sample.L, sample.L) > 0.0f)
      sample.initialized = true;
  }
}

Vec3f MCSubsurf::shade_incident(Ray& r, const Vec3f& normal) const
{
  return MCVolume::shade_new_ray(r);
}

bool MCSubsurf::trace_inside(const Medium* m, const Ray& r, const PositionSample& sample, Ray& new_r, Vec3f& L_i) const
{
  // Find distance and direction
  new_r.direction = sample.pos - r.hit_pos;
  new_r.dist = length(new_r.direction);
  new_r.direction /= new_r.dist;

  // Set remaining ray properties
  new_r.hit_pos = sample.pos;
  new_r.hit_normal = sample.normal;
  new_r.hit_face_id = sample.hit_face_id;
  new_r.u = sample.u;
  new_r.v = sample.v;
  new_r.origin = r.hit_pos;
  new_r.ior = m->get_ior(mono)[0].real();
  new_r.trace_depth = r.trace_depth + 1;
  new_r.did_hit_diffuse = r.did_hit_diffuse;
  new_r.hit_object = r.hit_object;
  new_r.inside = true;  
  new_r.has_hit = true;

  // Get entering radiance
  L_i = sample.L;
  return true;
}

Vec3f MCSubsurf::diffusion(const Ray& r, const Medium* m, const DiffusionProperties& dp, const PositionSample& sample) const
{
  // Trace a sample ray through the medium to find a surface point where light is incident
  Ray new_r;
  Vec3f L_i;
  if(!trace_inside(m, r, sample, new_r, L_i))
    return Vec3f(0.0f);
    
  // Compute distances to dipole sources

  // Compute intensities for dipole sources

  // Compute diffusion part of the BSSRDF and evaluate the rendering equation 
  // (remember to divide by the probabilities of the sampled ray directions)

  return Vec3f(0.0f);
}

bool MCSubsurf::compute_diffusion_properties(Ray& r, const Medium* m, DiffusionProperties* dp) const
{
  // Trace refracted ray and check distance to next surface
  Ray refracted;
  if(!MCVolume::tracer->trace_refracted(r, refracted) || refracted.dist < 1.0/m->get_reduced_ext(mono)[0])
    return false;
  dp->out_dir = -refracted.direction;

  // Find normal and cosine term at the hit point where radiance is emergent (outgoing)
  Vec3f normal = r.hit_normal;
  double cos_theta = dot(-r.direction, normal);
  if(cos_theta < 0.0)
  {
    normal = -normal;
    cos_theta = -cos_theta;
  }

  // Find the cosine term on the inside where light emerges on the surface (cos_theta_o)
  double cos_theta_o = dot(dp->out_dir, normal);

  // Compute Frensel transmittance
  Vec3f R_o(fresnel_R(cos_theta_o, cos_theta, dp->ior[0].real(), dp->ior_o[0].real()),
            fresnel_R(cos_theta_o, cos_theta, dp->ior[1].real(), dp->ior_o[1].real()),
            fresnel_R(cos_theta_o, cos_theta, dp->ior[2].real(), dp->ior_o[2].real()));
  //Vec3f R_o(fresnel_R(cos_theta_o, dp->ior[0], dp->ior_o[0]),
  //          fresnel_R(cos_theta_o, dp->ior[1], dp->ior_o[1]),
  //          fresnel_R(cos_theta_o, dp->ior[2], dp->ior_o[2]));
  dp->fresnel_T_o = one - R_o;

  // Get absorption coefficient
  const Color<double>& absorption = m->get_absorption(rgb);
  Vec3f sigma_a(absorption[0]*scene_scale, absorption[1]*scene_scale, absorption[2]*scene_scale);

  // Compute P_1 diffusion coefficient
  const Color<double>& reduced_ext = m->get_reduced_ext(rgb);
  Vec3f ext_p(reduced_ext[0]*scene_scale, reduced_ext[1]*scene_scale, reduced_ext[2]*scene_scale);
  dp->reduced_mean_free = one/ext_p;
  dp->diffusion = dp->reduced_mean_free/3.0f;

  // Compute diffuse Fresnel reflectance and the Groenhuis parameter A
  Vec3f R_dr(fresnel_diffuse(dp->ior[0].real()/dp->ior_o[0].real()), 
             fresnel_diffuse(dp->ior[1].real()/dp->ior_o[0].real()), 
             fresnel_diffuse(dp->ior[2].real()/dp->ior_o[0].real()));
  Vec3f A = (one + R_dr)/(one - R_dr);

  // Compute dipole source displacements
  dp->extrapolation = A*dp->diffusion*4.0f;
  dp->corrected_mean_free = dp->reduced_mean_free + dp->extrapolation;
  dp->real_source = dp->reduced_mean_free*dp->reduced_mean_free;
  dp->virtual_source = dp->corrected_mean_free*dp->corrected_mean_free;

  // Compute effective transport coefficient (D should be used, see Eason et al. [1978] or Aronson and Corngold [1999])
  Vec3f sigma_tr_sqr = sigma_a/dp->diffusion;
  dp->transport = Vec3f(sqrt(sigma_tr_sqr[0]), sqrt(sigma_tr_sqr[1]), sqrt(sigma_tr_sqr[2]));

  // Get reduced scattering albedo
  const Color<double>& reduced_alb = m->get_reduced_alb(rgb);
  dp->reduced_alb = Vec3f(reduced_alb[0], reduced_alb[1], reduced_alb[2]);

  return true;
}
