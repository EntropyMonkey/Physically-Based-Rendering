// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2013
// Copyright (c) DTU Compute 2013

#include "CGLA/Vec3f.h"
#include "CGLA/Vec2f.h"
#include "Ray.h"
#include "mt_random.h"
#include "sampler.h"
#include "PathTracer.h"

using namespace CGLA;

void PathTracer::update_pixel(unsigned int x, unsigned int y, float sample_number, Vec3f& L) const
{
  Vec2f vp_pos = Vec2f(x + mt_random(), y + mt_random())*win_to_vp + lower_left;
  Ray r = scene->get_camera()->get_ray(vp_pos);

	L *= sample_number;
  if(trace(r))
  {
    const Shader* s = get_shader(r);
    if(s)
      L += s->shade(r);
  }
  else
    L += get_background(r.direction);
	L /= sample_number + 1.0f;
}

bool PathTracer::trace_cosine_weighted(const Ray& in, Ray& out) const
{
  out.origin = in.hit_pos;
  out.direction = sample_cosine_weighted(in.hit_normal);
  out.trace_depth = in.trace_depth + 1;
  out.did_hit_diffuse = in.did_hit_diffuse;
  return trace(out);
}

bool PathTracer::trace_hemisphere(const Ray& in, Ray& out) const
{
  out.origin = in.hit_pos;
  out.direction = sample_hemisphere(in.hit_normal);
  out.trace_depth = in.trace_depth + 1;
  out.did_hit_diffuse = in.did_hit_diffuse;
  return trace(out);  
}

bool PathTracer::trace_isotropic(const Ray& in, Ray& out, double distance) const
{
  // Determine the origin and direction of the outgoing ray

  out.trace_depth = in.trace_depth + 1;
  out.did_hit_diffuse = in.did_hit_diffuse;
  return trace(out);
}

bool PathTracer::trace_HG(const Ray& in, Ray& out, double distance, double g) const
{
  // Determine the origin and direction of the outgoing ray

  out.trace_depth = in.trace_depth + 1;
  out.did_hit_diffuse = in.did_hit_diffuse;
  return trace(out);
}

bool PathTracer::trace_Phong_distribution(const Ray& in, Ray& out) const
{
  out.origin = in.hit_pos;
  out.direction = sample_Phong_distribution(in.hit_normal, -in.direction, in.get_hit_material()->shininess);
  out.trace_depth = in.trace_depth + 1;
  out.did_hit_diffuse = in.did_hit_diffuse;
  return trace(out);
}

bool PathTracer::trace_Blinn_distribution(const Ray& in, Ray& out) const
{
  out.origin = in.hit_pos;
  out.direction = sample_Blinn_distribution(in.hit_normal, -in.direction, in.get_hit_material()->shininess);
  out.trace_depth = in.trace_depth + 1;
  out.did_hit_diffuse = in.did_hit_diffuse;
  return trace(out);
}
