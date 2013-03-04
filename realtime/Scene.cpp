// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "CGLA/Vec3f.h"
#include "../optprops/Medium.h"
#include "../optprops/load_mpml.h"
#include "TriMesh.h"
#include "obj_load.h"
#include "ObjMaterial.h"
#include "Scene.h"
#include "AreaLight.h"
#include "Mirror.h"

using namespace std;
using namespace CGLA;

Scene::~Scene()
{
  for(unsigned int i = 0; i < meshes.size(); ++i)
    delete meshes[i];
  for(unsigned int i = 0; i < light_meshes.size(); ++i)
    delete light_meshes[i];
  for(unsigned int i = 0; i < extracted_lights.size(); ++i)
    delete lights[extracted_lights[i]];
}

void Scene::load_mesh(std::string filename, const Mat4x4f& transform)
{
  cout << "Loading " << filename << endl;

  TriMesh* mesh = new TriMesh; 
  obj_load(filename, *mesh);
  if(!mesh->has_normals())
  {
    cout << "Computing normals" << endl;
    mesh->compute_normals();
  }
  mesh->transform(transform);
  cout << "No. of triangles: " << mesh->geometry.no_faces() << endl;
  meshes.push_back(mesh);
  transforms.push_back(identity_Mat4x4f());

  // Correct scene bounding box
  AABB mesh_bbox = mesh->compute_bbox();
  bbox.add_AABB(mesh_bbox);
}

void Scene::load_media(string filename)
{
  Medium air;
  air.get_ior(mono).resize(1);
  air.get_ior(mono)[0] = complex<double>(1.0, 0.0);
  air.fill_rgb_data();
  air.name = "default";
  air.turbid = false;
  set_medium(air);

  load_mpml(filename, media, interfaces);
  for(unsigned int i = 0; i < meshes.size(); ++i)
    for(unsigned int j = 0; j < meshes[i]->materials.size(); ++j)
    {
      string name = meshes[i]->materials[j].name;
      const Interface& iface = interfaces[name];
      if(iface.name != "")
      {
        iface.med_in->fill_rgb_data();
        iface.med_in->fill_mono_data();
        if(iface.med_out)
        {
          iface.med_out->fill_rgb_data();
          iface.med_out->fill_mono_data();
        }
      }
    }
}

unsigned int Scene::extract_area_lights()
{
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
      lights.push_back(new AreaLight(mesh));
    }
  }
  return lights.size();
}

void Scene::extract_specular_objs()
{
  for(unsigned int i = 0; i < meshes.size(); ++i)
  {
    const vector<int>& indices = meshes[i]->mat_idx;
    for(unsigned int j = 0; j < indices.size(); ++j)
    {
      int illum = meshes[i]->materials[indices[j]].illum;
      if(illum == 3 || illum == 4 || illum == 11)
      {
        speculars[meshes[i]] = shaders[illum];
        break;
      }
    }
  }
}

void Scene::set_medium(const Medium& m) 
{ 
  media[m.name] = m;
  Medium& mm = media[m.name];
  mm.fill_rgb_data();
  mm.fill_mono_data();
  interfaces[m.name].med_in = &mm;
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

void Scene::set_shaders(Shader* s)
{
  for(unsigned int i = 0; i < shaders.size(); ++i)
    shaders[i] = s;
}

Shader* Scene::get_shader(const ObjMaterial* m)
{
  if(m)
  {
    unsigned int model = m->illum;
    if(model < shaders.size())
      return shaders[model];
  }
  return 0;
}

void Scene::get_bsphere(Vec3f &c, float &r) const
{
  Vec3f rad = bbox.get_diagonal()*0.5f;
  c = bbox.get_lower_left_far() + rad;
  r = rad.length();
}

void Scene::draw_mesh(const TriMesh* mesh)
{
  unsigned int faces = mesh->geometry.no_faces();
  const ObjMaterial* m = &mesh->materials[mesh->mat_idx[0]];
  Shader* shader = get_shader(m);
  if(shader)
  {
    Mirror* mirror = dynamic_cast<Mirror*>(shader);
    if(mirror)
      mirror->set_current_map(mesh);
    shader->enable();
    shader->set_material(m);
  }
  glBegin(GL_TRIANGLES);
  for(unsigned int i = 0; i < faces; ++i)
  {
    const ObjMaterial* new_m = &mesh->materials[mesh->mat_idx[i]];
    if(new_m != m)
    {
      glEnd();
      m = new_m;
      Shader* new_shader = get_shader(m);
      if(new_shader != shader)
      {
        if(shader)
          shader->disable();
        shader = new_shader;
        if(shader)
          shader->enable();
      }
      if(shader)
        shader->set_material(m);
      glBegin(GL_TRIANGLES);
    }
    Vec3i n_face = mesh->normals.face(i);
    Vec3i g_face = mesh->geometry.face(i);
    for(int j = 0; j < 3; ++j)
    {
      if(mesh->normals.no_faces() > i)
      {
        Vec3f norm = normalize(mesh->normals.vertex(n_face[j]));
        glNormal3fv(norm.get());

        if(shader)
          shader->set_attributes(mesh, g_face[j]);
      }
      Vec3f vert = mesh->geometry.vertex(g_face[j]);  
      glColor3fv(m->diffuse);
      glVertex3fv(vert.get());
    }
  }
  glEnd();
  if(shader)
    shader->disable();
}

void Scene::gen_disp_list(unsigned int& disp_list)
{
  if(glIsList(disp_list))
    glDeleteLists(disp_list, 1);
  disp_list = glGenLists(1);
  glNewList(disp_list, GL_COMPILE);

  for(unsigned int i = 0; i < meshes.size(); ++i)
    draw_mesh(meshes[i]);

  glEndList();
}

void Scene::draw_reflection_maps()
{
  map<const TriMesh*, Shader*>::const_iterator i = speculars.begin();
  for(; i != speculars.end(); ++i)
  {
    Mirror* mirror = dynamic_cast<Mirror*>(i->second);
    if(mirror)
      mirror->generate_map(i->first);
  }
}

void Scene::draw()
{
  static unsigned int disp_list = 0;

  if(redraw)
  {
    redraw = false;
    cout << "Generating scene display list" << endl;
    gen_disp_list(disp_list);
  }

  glCallList(disp_list);
}
