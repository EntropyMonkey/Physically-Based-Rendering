// Written by Jeppe Revall Frisvad, 2011
// Copyright (c) DTU Informatics 2011

#include <vector>
#include "Ray.h"
#include "AccObj.h"
#include "Object3D.h"
#include "Plane.h"
#include "Accelerator.h"

using namespace std;

Accelerator::~Accelerator()
{
  for(unsigned int i = 0; i < primitives.size(); ++i)
    delete primitives[i];
}

void Accelerator::init(const vector<const TriMesh*>& geometry, const vector<const Plane*>& scene_planes)
{
  for(unsigned int i = 0; i < geometry.size(); ++i)
  {
    const TriMesh* obj = geometry[i];
    unsigned int no_of_prims = primitives.size();
    primitives.resize(no_of_prims + obj->get_no_of_primitives());
    for(unsigned int j = 0; j < obj->get_no_of_primitives(); ++j)
      primitives[j + no_of_prims] = new AccObj(obj, j);
  }
  planes = scene_planes;
}

bool Accelerator::closest_hit(Ray& r) const
{
  closest_plane(r);
  for(unsigned int i = 0; i < primitives.size(); ++i)
  {
    AccObj* obj = primitives[i];
    if(obj->geometry->intersect(r, obj->prim_idx))
      r.tmax = r.dist;
  }
  if(r.has_hit)
    r.hit_pos = r.origin + r.dist*r.direction;  
  return r.has_hit;
}

bool Accelerator::any_hit(Ray& r) const
{
  if(!any_plane(r))
  {
    unsigned int i = 0;
    while(i < primitives.size() && !r.has_hit)
    {
      AccObj* obj = primitives[i++];
      obj->geometry->intersect(r, obj->prim_idx);
    }
  }
  return r.has_hit;
}

void Accelerator::closest_plane(Ray& r) const
{
  for(unsigned int i = 0; i < planes.size(); ++i)
    if(planes[i]->intersect(r, 0))
      r.tmax = r.dist;
}

bool Accelerator::any_plane(Ray& r) const
{
  for(unsigned int i = 0; i < planes.size(); ++i)
    if(planes[i]->intersect(r, 0))
      return true;
  return false;
}
