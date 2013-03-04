// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2010
// Copyright (c) DTU Informatics 2010

#ifndef MCSUBSURF_H
#define MCSUBSURF_H

#include <vector>
#include <map>
#include "CGLA/Vec3f.h"
#include "../optprops/Interface.h"
#include "../optprops/Medium.h"
#include "Ray.h"
#include "TriMesh.h"
#include "PathTracer.h"
#include "Light.h"
#include "MCVolume.h"
#include "MCLambertian.h"
#include "PositionSample.h"

struct DiffusionProperties
{
  DiffusionProperties(const Color< std::complex<double> >& m_ior, const Color< std::complex<double> >& ior_out)
    : ior(m_ior), ior_o(ior_out)
  { }

  const Color< std::complex<double> >& ior;   // refractive index of scattering medium
  const Color< std::complex<double> >& ior_o; // refractive index of medium that the emergent light refracts into
  CGLA::Vec3f out_dir;                        // omega_21, direction that refracts to omega_o
  CGLA::Vec3f fresnel_T_o;                    // Fresnel transmittance from inside out
  CGLA::Vec3f reduced_alb;                    // alpha'
  CGLA::Vec3f reduced_mean_free;              // 1/sigma_t'
  CGLA::Vec3f diffusion;                      // D
  CGLA::Vec3f extrapolation;                  // 2*A*D
  CGLA::Vec3f corrected_mean_free;            // 1/sigma_t' + 4*A*D
  CGLA::Vec3f transport;                      // sqrt(sigma_a/D)
  CGLA::Vec3f real_source;                    // squared displacement of real source   : (1/sigma_t')^2
  CGLA::Vec3f virtual_source;                 // squared displacement of virtual source: (1/sigma_t' + 4*A*D)^2
};

class MCSubsurf : public MCVolume, public MCLambertian
{
public:
  MCSubsurf(PathTracer* pathtracer,
            const std::vector<Light*>& light_vector,
            unsigned int no_of_diffusion_samples = 1,
            int no_of_splits = 1) 
    : MCVolume(pathtracer, no_of_splits),
      MCLambertian(pathtracer, light_vector, no_of_diffusion_samples)
  { 
    use_multiple_scattering = false;
  }

  virtual CGLA::Vec3f shade(Ray& r, bool emit = true) const;

  void init_sample_surface(const TriMesh* mesh, const Interface* m);
  void clear_surface_samples(const TriMesh* mesh) { pos_samples[mesh].clear(); }

protected:
  bool trace_inside(const Medium* m, const Ray& r, const PositionSample& sample, Ray& new_r, CGLA::Vec3f& L_i) const;
  CGLA::Vec3f shade_incident(Ray& r, const CGLA::Vec3f& normal) const;
  CGLA::Vec3f diffusion(const Ray& r, const Medium* m, const DiffusionProperties& dp, const PositionSample& sample) const;
  bool compute_diffusion_properties(Ray& r, const Medium* m, DiffusionProperties* dp) const;
  double fresnel_diffuse(double n) const { return -1.440/(n*n) + 0.710/n + 0.668 + 0.0636*n; }

  std::map<const TriMesh*, std::vector<PositionSample> > pos_samples;
};

#endif // MCSUBSURF_H
