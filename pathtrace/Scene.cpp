// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2013
// Copyright (c) DTU Compute 2013

#include <iostream>
#include <string>
#include <vector>
#include <GL/glut.h>
#include "CGLA/Vec3f.h"
#include "../optprops/Interface.h"
#include "../optprops/Medium.h"
#include "../optprops/load_mpml.h"
#include "TriMesh.h"
#include "obj_load.h"
#include "BSPTree.h"
#include "ObjMaterial.h"
#include "Ray.h"
#include "AreaLight.h"
#include "RayTracer.h"
#include "Plane.h"
#include "Texture.h"
#include "Scene.h"

using namespace std;
using namespace CGLA;

namespace
{
  const int MAX_OBJECTS = 4;   // Maximum number of triangles in a BSP tree node
  const int MAX_LEVEL = 20;    // Maximum number of BSP tree subdivisions
}

Scene::~Scene()
{
  for(unsigned int i = 0; i < meshes.size(); ++i)
    delete meshes[i];
  for(unsigned int i = 0; i < light_meshes.size(); ++i)
    delete light_meshes[i];
  for(unsigned int i = 0; i < extracted_lights.size(); ++i)
    delete lights[extracted_lights[i]];
  for(map<string, Texture*>::iterator i = textures.begin(); i != textures.end(); ++i)
    delete i->second;
  for(unsigned int i = 0; i < planes.size(); ++i)
    delete planes[i];
}

void Scene::load_mesh(const string& filename, const Mat4x4f& transform)
{
  cout << "Loading " << filename << endl;
  TriMesh* mesh = load_background_mesh(filename, transform);

  // Correct scene bounding box
  AABB mesh_bbox = mesh->compute_bbox();
  bbox.add_AABB(mesh_bbox);
}

TriMesh* Scene::load_background_mesh(const string& filename, const Mat4x4f& transform)
{
  TriMesh* mesh = new TriMesh; 
  obj_load(filename, *mesh);
  if(!mesh->has_normals())
  {
    cout << "Computing normals" << endl;
    mesh->compute_normals();
  }
  mesh->transform(transform);
  mesh->compute_areas();
  cout << "No. of triangles: " << mesh->geometry.no_faces() << endl;
  meshes.push_back(mesh);
  return mesh;
}

void Scene::load_media(const string& filename)
{
  Medium air;
  air.get_ior(mono).resize(1);
  air.get_ior(mono)[0] = complex<double>(1.0, 0.0);
  air.fill_rgb_data();
  air.name = "default";
  set_medium(air);

  for(unsigned int i = 0; i < planes.size(); ++i)
    meshes.push_back(planes[i]->get_mesh());
  load_mpml(filename, media, interfaces);
  for(unsigned int i = 0; i < meshes.size(); ++i)
    for(unsigned int j = 0; j < meshes[i]->materials.size(); ++j)
    {
      const ObjMaterial& mat = meshes[i]->materials[j];
      string name = mat.name;
      Interface& iface = interfaces[name];
      if(!iface.med_in)
      {
        iface.name = name;
        iface.med_in = &media[name];
        iface.med_in->name = name;
        Color< complex<double> >& ior = iface.med_in->get_ior(mono);
        ior.resize(1);
        ior[0] = complex<double>(mat.ior, 0.0);
        float emissive = 0.0f;
        for(unsigned int k = 0; k < 3; ++k)
          emissive += mat.ambient[k];
        if(emissive > 0.0)
        {
          iface.med_in->emissive = true;
          Color<double>& emission = iface.med_in->get_emission(rgb);
          emission.resize(3);
          for(unsigned int k = 0; k < emission.size(); ++k)
            emission[k] = mat.ambient[k];
        }
      }
      iface.med_in->fill_rgb_data();
      iface.med_in->fill_mono_data();
      if(iface.med_out)
      {
        iface.med_out->fill_rgb_data();
        iface.med_out->fill_mono_data();
      }
      if(mat.has_texture)
      {
        Texture*& tex = textures[mat.tex_name];
        tex = new Texture;
        string path_and_name = mat.tex_path + mat.tex_name;
        tex->load(path_and_name.c_str());
      }
    }
  for(unsigned int i = 0; i < planes.size(); ++i)
    meshes.pop_back();
}

void Scene::add_plane(const Vec3f& position, const Vec3f& normal, const string& mtl_file, unsigned int mtl_idx, float tex_scale)
{
  vector<ObjMaterial> m;
  if(!mtl_file.empty())
    mtl_load(mtl_file, m);
  if(m.size() == 0)
	  m.push_back(ObjMaterial());
  planes.push_back(new Plane(position, normal, mtl_idx < m.size() ? m[mtl_idx] : m.back(), tex_scale));
}

unsigned int Scene::extract_area_lights(RayTracer* tracer, unsigned int samples_per_light)
{
  light_tracer = tracer;
  for(unsigned int i = 0; i < meshes.size(); ++i)
  {
    TriMesh* mesh = new TriMesh;
    const vector<int>& indices = meshes[i]->mat_idx;
    for(unsigned int j = 0; j < indices.size(); ++j)
    {
      const ObjMaterial& mat = meshes[i]->materials[indices[j]];
      if(mat.name == "default")
        continue;

      bool emissive = false;
      for(unsigned int k = 0; k < 3; ++k)
        emissive = emissive || (meshes[i]->materials[indices[j]].ambient[k] > 0.0f);

      if(emissive)
      {
    	  Vec3i g_face = meshes[i]->geometry.face(j);
        for(unsigned int k = 0; k < 3; ++k)
          g_face[k] = mesh->geometry.add_vertex(meshes[i]->geometry.vertex(g_face[k]));
        int idx = mesh->geometry.add_face(g_face);
        if(meshes[i]->normals.no_faces() > j)
        {        
          Vec3i n_face = meshes[i]->normals.face(j);
          for(unsigned int k = 0; k < 3; ++k)
	          n_face[k] = mesh->normals.add_vertex(meshes[i]->normals.vertex(n_face[k]));
          mesh->normals.add_face(n_face, idx);
        }
        if(meshes[i]->texcoords.no_faces() > j)
        {
          Vec3i t_face = meshes[i]->texcoords.face(j);
          for(unsigned int k = 0; k < 3; ++k)
  	        t_face[k] = mesh->texcoords.add_vertex(meshes[i]->texcoords.vertex(t_face[k]));
          mesh->texcoords.add_face(t_face, idx);
        }
        
        bool material_exists = false;
        for(unsigned int k = 0; k < mesh->materials.size(); ++k)
          if(mesh->materials[k].name == meshes[i]->materials[indices[j]].name)
          {
            mesh->mat_idx.push_back(k);
            material_exists = true;
            break;
          }
        if(!material_exists)
        {
          mesh->mat_idx.push_back(mesh->materials.size());
          mesh->materials.push_back(meshes[i]->materials[indices[j]]);
        }
      }
    }
    if(mesh->geometry.no_faces() == 0)
      delete mesh;
    else
    {
      mesh->compute_areas();
      light_meshes.push_back(mesh);
      extracted_lights.push_back(lights.size());
      lights.push_back(new AreaLight(light_tracer, mesh, samples_per_light));
    }
  }
  return lights.size();
}

void Scene::toggle_shadows()
{
  for(unsigned int i = 0; i < lights.size(); ++i)
    lights[i]->toggle_shadows();
}

void Scene::set_medium(const Medium& m) 
{ 
  media[m.name] = m;
  Medium& mm = media[m.name];
  mm.fill_rgb_data();
  mm.fill_mono_data();
  Interface& iface = interfaces[m.name];
  iface.name = m.name;
  iface.med_in = &mm;
}

void Scene::set_shader(int model, Shader* s)
{
  if(model < 0)
  {
    cout << "Negative identification numbers are not supported for illumination models." << endl;
    return;
  }
  while(model >= static_cast<int>(shaders.size()))
    shaders.push_back(0);
  shaders[model] = s;
}

const Shader* Scene::get_shader(const Ray& r) const
{
  unsigned int model = r.get_hit_material()->illum;
  if(model < shaders.size())
    return shaders[model];
  return 0;
}

map<const TriMesh*, const Interface*> Scene::get_volumes()
{
  map<const TriMesh*, const Interface*> volumes;
  for(unsigned int i = 0; i < meshes.size(); ++i)
  {
    const TriMesh* mesh = meshes[i];
    const ObjMaterial* m = 0;
    if(mesh->mat_idx.size() > 0)
      m = &meshes[i]->materials[meshes[i]->mat_idx[0]];
    if(m && (m->illum > 13 && m->illum <= 17))
      volumes[meshes[i]] = &get_interface(m);
  }
  return volumes;
}

void Scene::get_bsphere(CGLA::Vec3f &c, float &r) const
{
  Vec3f rad = bbox.get_diagonal()*0.5f;
  c = bbox.get_lower_left_far() + rad;
  r = rad.length();
}

void Scene::draw_mesh(const TriMesh* mesh) const
{
  vector<Vec3f*> shades(mesh->geometry.no_vertices(), static_cast<Vec3f*>(0));
  unsigned int faces = mesh->geometry.no_faces();
  glBegin(GL_TRIANGLES);
  for(unsigned int i = 0; i < faces; ++i)
  {
    Vec3i g_face = mesh->geometry.face(i);
    for(int j = 0; j < 3; ++j)
    {
      Vec3f vert = mesh->geometry.vertex(g_face[j]);	  
      Vec3f shade(0.5f);

      if(mesh->normals.no_faces() > i)
      {
        Vec3i n_face = mesh->normals.face(i);
        Vec3f norm = normalize(mesh->normals.vertex(n_face[j]));
        glNormal3fv(norm.get());

        if(!shades[g_face[j]])
        {
          Vec3f ray_vec = vert - cam->get_position();
          float dist = length(ray_vec);
          Ray r(cam->get_position(), ray_vec/dist);
          r.hit_object = mesh;
          r.hit_face_id = i;
          unsigned int model = r.get_hit_material()->illum;
          if(model < shaders.size() && shaders[model])
          {
            r.hit_pos = vert;
            r.hit_normal = norm;
            r.has_hit = true;
            r.dist = dist;
            shade = shaders[model]->shade(r);
          }
        }
      }
      if(shades[g_face[j]])
        shade = *shades[g_face[j]];
      else
        shades[g_face[j]] = new Vec3f(shade);

      glColor3fv(shade.get());
      glVertex3fv(vert.get());
    }
    if(faces > 100 && (i + 1) % (faces/10) == 0)
      cout << ".";
  }
  glEnd();

  for(unsigned int i = 0; i < shades.size(); ++i)
    delete shades[i];
}

void Scene::draw_plane(const Plane* plane)
{
  Vec3f c;
  float rad;
  get_bsphere(c, rad);
  rad *= 100.0f;
  Vec3f shade(0.5f);
  Vec3f vert = plane->get_origin();
  Vec3f v0 = vert - rad*plane->get_tangent() - rad*plane->get_binormal();
  Vec3f v1 = vert + rad*plane->get_tangent() - rad*plane->get_binormal();
  Vec3f v2 = vert - rad*plane->get_tangent() + rad*plane->get_binormal();
  Vec3f v3 = vert + rad*plane->get_tangent() + rad*plane->get_binormal();
  Vec3f norm = plane->get_normal();
  Vec3f ray_vec = vert - cam->get_position();
  float dist = length(ray_vec);
  Ray r(cam->get_position(), ray_vec/dist);
  r.hit_object = plane->get_mesh();
  r.hit_face_id = 0;
  const ObjMaterial* mat = r.get_hit_material();
  unsigned int model = mat->illum;
  if(model < shaders.size() && shaders[model])
  {
    r.hit_pos = vert;
    r.hit_normal = norm;
    r.has_hit = true;
    r.dist = dist;
    plane->get_uv(vert, r.u, r.v);
    shade = shaders[model]->shade(r);
  }
  const Texture* tex = do_textures && mat->has_texture ? textures[mat->tex_name] : 0;
  if(tex)
  {
    tex->enable();
    tex->bind();
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  }
  glBegin(GL_TRIANGLE_STRIP);
    glColor3fv(shade.get());
    glNormal3fv(norm.get());
    Vec2f uv;
    plane->get_uv(v0, uv[0], uv[1]);
    glTexCoord2fv(uv.get());
    glVertex3fv(v0.get());
    plane->get_uv(v1, uv[0], uv[1]);
    glTexCoord2fv(uv.get());
    glVertex3fv(v1.get());
    plane->get_uv(v2, uv[0], uv[1]);
    glTexCoord2fv(uv.get());
    glVertex3fv(v2.get());
    plane->get_uv(v3, uv[0], uv[1]);
    glTexCoord2fv(uv.get());
    glVertex3fv(v3.get());
  glEnd();
  if(tex)
    tex->disable();
}

void Scene::draw()
{
  static unsigned int disp_list = 0;

  if(redraw)
  {
    cout << "Generating scene display list";
    if(glIsList(disp_list))
      glDeleteLists(disp_list, 1);
    disp_list = glGenLists(1);
    glNewList(disp_list, GL_COMPILE);

    for(unsigned int i = 0; i < meshes.size(); ++i)
      draw_mesh(meshes[i]);
    for(unsigned int i = 0; i < planes.size(); ++i)
      draw_plane(planes[i]);

    glEndList();
    redraw = false;
    cout << endl;
  }

  glCallList(disp_list);
}

bool Scene::is_specular(const ObjMaterial* m) const
{
  return m && ((m->illum > 2 && m->illum < 10) || m->illum > 10);
}
