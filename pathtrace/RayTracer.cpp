// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2013
// Copyright (c) DTU Compute 2013

#include <complex>
#include "CGLA/Vec3f.h"
#include "../optprops/Medium.h"
#include "Ray.h"
#include "fresnel.h"
#include "RayTracer.h"

using namespace std;
using namespace CGLA;

bool RayTracer::trace_reflected(const Ray& in, Ray& out) const
{
  out.origin = in.hit_pos;
  out.direction = 2.0*dot(-in.direction, in.hit_normal)*in.hit_normal + in.direction;
  out.ior = in.ior;
  out.trace_depth = in.trace_depth + 1;
  out.did_hit_diffuse = in.did_hit_diffuse;
  return trace(out);
}

bool RayTracer::trace_reflected(const Ray& in, Ray& out, double& R) const
{
  Vec3f normal, direction;
  double cos_theta_in;
  float ior2 = get_ior_out(in, direction, normal, cos_theta_in);
  R = fresnel_R(cos_theta_in, in.ior, ior2);
  out.origin = in.hit_pos;
  out.direction = 2.0*cos_theta_in*normal - direction;
  out.ior = in.ior;
  out.trace_depth = in.trace_depth + 1;
  out.did_hit_diffuse = in.did_hit_diffuse;
  return trace(out);  
}

bool RayTracer::trace_reflected(const Ray& in, Ray& out, Vec3f& R) const
{
  Vec3f normal = in.hit_normal;
  Vec3f direction = -in.direction;
  double cos_theta_in = dot(normal, direction);
  bool from_inside = cos_theta_in < 0.0;
	if(from_inside)
	{
    normal = -normal;
		cos_theta_in = -cos_theta_in;
  }
  const Medium* m1 = get_hit_medium(in, !from_inside);
  const Medium* m2 = get_hit_medium(in, from_inside);
  const Color< complex<double> >& ior1 = m1->get_ior(rgb);
  const Color< complex<double> >& ior2 = m2->get_ior(rgb);
  for(int i = 0; i < 3; ++i)
    R[i] = fresnel_R(cos_theta_in, ior1[i], ior2[i]);
  out.origin = in.hit_pos;
  out.direction = 2.0*cos_theta_in*normal - direction;
  out.ior = in.ior;
  out.trace_depth = in.trace_depth + 1;
  out.did_hit_diffuse = in.did_hit_diffuse;
  return trace(out);  
}

bool RayTracer::trace_refracted(const Ray& in, Ray& out) const
{
  Vec3f normal, direction;
  double cos_theta_in;
  out.ior = get_ior_out(in, direction, normal, cos_theta_in);
  double eta = in.ior/out.ior;
  double cos_theta_out_sqr = 1.0 - eta*eta*(1.0 - cos_theta_in*cos_theta_in);
  if(cos_theta_out_sqr < 0.0)
    return false;

  double cos_theta_out = std::sqrt(cos_theta_out_sqr);
  out.origin = in.hit_pos;
  out.direction = eta*(normal*cos_theta_in - direction) - normal*cos_theta_out;
  out.trace_depth = in.trace_depth + 1;
  out.did_hit_diffuse = in.did_hit_diffuse;
  return trace(out);
}

bool RayTracer::trace_refracted(const Ray& in, Ray& out, double& R) const
{
  Vec3f normal, direction;
  double cos_theta_in;
  out.ior = get_ior_out(in, direction, normal, cos_theta_in);
  double eta = in.ior/out.ior;
  double cos_theta_out_sqr = 1.0 - eta*eta*(1.0 - cos_theta_in*cos_theta_in);
  if(cos_theta_out_sqr < 0.0)
  {
    R = 1;
    return false;
  }

  double cos_theta_out = std::sqrt(cos_theta_out_sqr);
  out.origin = in.hit_pos;
  out.direction = eta*(normal*cos_theta_in - direction) - normal*cos_theta_out;
  out.trace_depth = in.trace_depth + 1;
  out.did_hit_diffuse = in.did_hit_diffuse;
  R = fresnel_R(cos_theta_in, cos_theta_out, in.ior, out.ior);
  return trace(out);
}

bool RayTracer::trace_dispersive(const Ray& in, Ray& out, double& R, unsigned int color_band, ColorType color_type) const
{
  Vec3f normal, direction;
  double cos_theta_in;
  out.ior = get_ior_out(in, direction, normal, cos_theta_in, color_band, color_type);
  double eta = in.ior/out.ior;
  double cos_theta_out_sqr = 1.0 - eta*eta*(1.0 - cos_theta_in*cos_theta_in);
  if(cos_theta_out_sqr < 0.0)
  {
    R = 1;
    return false;
  }

  double cos_theta_out = std::sqrt(cos_theta_out_sqr);
  out.origin = in.hit_pos;
  out.direction = eta*(normal*cos_theta_in - direction) - normal*cos_theta_out;
  out.trace_depth = in.trace_depth + 1;
  out.did_hit_diffuse = in.did_hit_diffuse;
  R = fresnel_R(cos_theta_in, cos_theta_out, in.ior, out.ior);
  return trace(out);
}

double RayTracer::get_ior_out(const Ray& in, Vec3f& dir, Vec3f& normal, double& cos_theta_in) const
{
  normal = in.hit_normal;
  dir = -in.direction;
  cos_theta_in = dot(normal, dir);
  bool from_inside = cos_theta_in < 0.0;
  const Medium* m = get_hit_medium(in, from_inside);
	if(from_inside)
	{
    normal = -normal;
		cos_theta_in = -cos_theta_in;
  }
  return m->get_ior(mono)[0].real();
}

double RayTracer::get_ior_out(const Ray& in, Vec3f& dir, Vec3f& normal, double& cos_theta_in, unsigned int color_band, ColorType color_type) const
{
  normal = in.hit_normal;
  dir = -in.direction;
  cos_theta_in = dot(normal, dir);
  bool from_inside = cos_theta_in < 0.0;
  const Medium* m = get_hit_medium(in, from_inside);
	if(from_inside)
	{
    normal = -normal;
		cos_theta_in = -cos_theta_in;
  }
  unsigned int max_band = m->get_ior(color_type).size();
  if(max_band == 0)
    return m->get_ior(mono)[0].real();
  else if(color_band < max_band)
    return m->get_ior(color_type)[color_band].real();
  else
    return m->get_ior(color_type)[max_band - 1].real();
}
