// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2013
// Copyright (c) DTU Compute 2013

#include <iostream>
#include "CGLA/Vec3d.h"
#include "CGLA/Vec2f.h"
#include "Ray.h"
#include "mt_random.h"
#include "Shader.h"
#include "RayCaster.h"

using namespace std;
using namespace CGLA;

void RayCaster::compute_jitters()
{
  float aspect = static_cast<float>(width)/static_cast<float>(height);
  win_to_vp[0] = aspect/static_cast<float>(width);
  win_to_vp[1] = 1.0f/static_cast<float>(height);
  lower_left = Vec2f(aspect*(-0.5f), -0.5f);
  step = win_to_vp/static_cast<float>(subdivs);

  jitter.resize(subdivs*subdivs, Vec2f(0.5f)*win_to_vp);
  for(unsigned int i = 0; i < subdivs; ++i)
    for(unsigned int j = 1; j < subdivs; ++j)
      jitter[i*subdivs + j] = Vec2f(safe_mt_random() + j, safe_mt_random() + i)*step; 
}

void RayCaster::increment_pixel_subdivs()
{
  ++subdivs;
  compute_jitters();
  cout << "Rays per pixel: " << subdivs*subdivs << endl;
}

void RayCaster::decrement_pixel_subdivs()
{
  if(subdivs > 1)
  {
    --subdivs;
    compute_jitters();
  }
  cout << "Rays per pixel: " << subdivs*subdivs << endl;
}

Vec3f RayCaster::compute_pixel(unsigned int x, unsigned int y) const
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
    }
    else
      result += get_background(r.direction);
  }
  return result/static_cast<float>(jitter.size());
}

Vec3f RayCaster::get_background(const Vec3f& dir) const
{ 
  if(sphere_tex)
    return Vec3f(sphere_tex->sample_linear(dir));
  if(sun_sky)
    return sun_sky->skyColor(dir);
  return background;
}
