// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2013
// Copyright (c) DTU Compute 2013

#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <string>
#include <map>
#include "CGLA/Vec3f.h"
#include "CGLA/Mat4x4f.h"
#include "../optprops/Interface.h"
#include "../optprops/Medium.h"
#include "TriMesh.h"
#include "BSPTree.h"
#include "Ray.h"
#include "ObjMaterial.h"
#include "Light.h"
#include "Camera.h"
#include "Shader.h"
#include "AABB.h"
#include "Plane.h"
#include "Texture.h"

class RayTracer;

class Scene
{
public:
  Scene(const Camera* c) : planes(0), light_tracer(0), cam(c), shaders(10, static_cast<Shader*>(0)), redraw(true), do_textures(false) { }
  ~Scene();

  // Accessors
  void set_shader(int model, Shader* s);
  const std::vector<Shader*>& get_shaders() const { return shaders; }
  const Shader* get_shader(const Ray& r) const;
  const std::vector<Light*>& get_lights() const { return lights; }
  const std::vector<const TriMesh*>& get_meshes() const { return meshes; }
  const Camera* get_camera() const { return cam; }
  void get_bsphere(CGLA::Vec3f& c, float& r) const;
  Interface& get_interface(const ObjMaterial* m) { return interfaces[m->name]; }
  Interface& get_interface(const std::string& name) { return interfaces[name]; }
  Medium& get_medium(const ObjMaterial* m) { return media[m->name]; }
  Medium& get_medium(const std::string& name) { return media[name]; }
  void set_medium(const Medium& m);
  std::map<std::string, Texture*>& get_textures() { return textures; }
  std::map<const TriMesh*, const Interface*> get_volumes();

  // Loaders
  void load_mesh(const std::string& filename, const CGLA::Mat4x4f& transform = CGLA::identity_Mat4x4f());
  TriMesh* load_background_mesh(const std::string& filename, const CGLA::Mat4x4f& transform = CGLA::identity_Mat4x4f());
  void load_media(const std::string& filename);
  void add_plane(const CGLA::Vec3f& position, const CGLA::Vec3f& normal, const std::string& mtl_file, unsigned int mtl_idx = 1, float tex_scale = 1.0f);

  // Light handling
  void add_light(Light* light) { if(light) lights.push_back(light); }
  unsigned int extract_area_lights(RayTracer* tracer, unsigned int samples_per_light = 1);
  void reset_light_tracer(RayTracer* tracer) { light_tracer = tracer; }
  void toggle_shadows();

  // Draw
  void draw();
  void textures_on() { do_textures = true; redraw = true; }
  void redo_display_list() { redraw = true; }
  bool is_redoing_display_list() { return redraw; }

  // Ray intersection
  void build_bsptree() { tree.init(meshes, planes); }
  bool intersect(Ray& r) const { return tree.closest_hit(r); }
  bool intersect_light(const Ray& r, CGLA::Vec3f& L) { return lights.size() > 0 ? lights[0]->intersect(r, L) : false; }

  // ObjMaterial classification
  bool is_specular(const ObjMaterial* m) const;

private:
  void draw_mesh(const TriMesh* mesh) const;
  void draw_plane(const Plane* plane);

  std::map<std::string, Medium> media;
  std::map<std::string, Interface> interfaces;
  std::map<std::string, Texture*> textures;
  std::vector<Light*> lights;
  std::vector<const TriMesh*> light_meshes;
  std::vector<unsigned int> extracted_lights;
  std::vector<const TriMesh*> meshes;
  std::vector<const Plane*> planes;
  RayTracer* light_tracer;
  BspTree tree;
  AABB bbox;
  const Camera* cam;
  std::vector<Shader*> shaders;
  bool redraw;
  bool do_textures;
};

#endif // SCENE_H

