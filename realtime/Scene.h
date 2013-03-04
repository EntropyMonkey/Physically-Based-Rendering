// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <string>
#include <map>
#include "CGLA/Vec3f.h"
#include "CGLA/Mat4x4f.h"
#include "TriMesh.h"
#include "ObjMaterial.h"
#include "../optprops/Medium.h"
#include "../optprops/Interface.h"
#include "Camera.h"
#include "AABB.h"
#include "Shader.h"
#include "Light.h"

class Scene
{
public:
  Scene(Camera* c) : cam(c), shaders(10, static_cast<Shader*>(0)), redraw(true) { }
  ~Scene();

  // Accessors
  void set_shader(int model, Shader* s);
  void set_shaders(Shader* s);
  void set_shaders(const std::vector<Shader*>& all_shaders) { shaders = all_shaders; }
  Shader* get_shader(const ObjMaterial* m);
  const std::vector<Shader*>& get_shaders() { return shaders; }
  const std::vector<const Light*>& get_lights() const { return lights; }
  const std::vector<const TriMesh*>& get_meshes() const { return meshes; }
  Camera* get_camera() { return cam; }
  void get_bsphere(CGLA::Vec3f& c, float& r) const;
  const AABB& get_bbox() const { return bbox; }
  Interface& get_interface(const ObjMaterial* m) { return interfaces[m->name]; }
  Interface& get_interface(std::string name) { return interfaces[name]; }
  Medium& get_medium(const ObjMaterial* m) { return media[m->name]; }
  Medium& get_medium(std::string name) { return media[name]; }
  void set_medium(const Medium& m);

  // Loaders
  void load_mesh(std::string filename, const CGLA::Mat4x4f& transform = CGLA::identity_Mat4x4f());
  void load_media(std::string filename);

  // Light handling
  void add_light(const Light* light) { if(light) lights.push_back(light); }
  unsigned int extract_area_lights();

  // Mirror handling
  void extract_specular_objs();
  void draw_reflection_maps();

  // Draw
  void gen_disp_list(unsigned int& disp_list);
  void draw();
  void redo_display_list() { redraw = true; }

private:
  void draw_mesh(const TriMesh* mesh);

  std::map<std::string, Medium> media;
  std::map<std::string, Interface> interfaces;
  std::vector<const Light*> lights;
  std::vector<const TriMesh*> light_meshes;
  std::vector<unsigned int> extracted_lights;
  std::vector<const TriMesh*> meshes;
  std::vector<CGLA::Mat4x4f> transforms;
  std::map<const TriMesh*, Shader*> speculars;
  AABB bbox;
  CGLA::Vec3f center;
  float radius;
  Camera* cam;
  std::vector<Shader*> shaders;
  bool redraw;
};

#endif // SCENE_H
