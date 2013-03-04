// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef TRACER_H
#define TRACER_H

#include "CGLA/Vec3f.h"
#include "../optprops/Medium.h"
#include "../optprops/Interface.h"
#include "Ray.h"
#include "Shader.h"
#include "Scene.h"

class Tracer
{
public:
  Tracer(unsigned int w, unsigned int h, Scene* s)
    : width(w), height(h), scene(s)
  { }

  void set_scene(Scene* s) { scene = s; }
  const Shader* get_shader(const Ray& r) const { return scene ? scene->get_shader(r) : 0; }
  const Medium* get_hit_medium(const Ray& r, bool from_inside) const
  {
    if(scene)
    {
      const ObjMaterial* m = r.get_hit_material();
      if(!m || m->name.length() == 0)
        return 0;

      Interface& iface = scene->get_interface(m);
      Medium* medium = from_inside ? iface.med_out : iface.med_in;
      return medium && medium->name.length() > 0 ? medium : &scene->get_medium("default");
    }
    return 0;
  }
  const Medium* get_medium(const Ray& r) const { return get_hit_medium(r, !r.inside); }
  const Medium* get_medium(const std::string& name) const { return scene ? &scene->get_medium(name) : 0; }
  void get_bsphere(CGLA::Vec3f& center, float& radius) { if(scene) scene->get_bsphere(center, radius); }
  
  bool intersect_light(const Ray& r, CGLA::Vec3f& L) { return scene->intersect_light(r, L); }

  virtual CGLA::Vec3f compute_pixel(unsigned int x, unsigned int y) const = 0;

protected:
  // Resolution
  unsigned int width;
  unsigned int height;

  // The scene
  Scene* scene;
};

#endif // TRACER_H
