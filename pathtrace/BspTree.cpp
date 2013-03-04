// 02562 Rendering Framework
// Inspired by BSP tree in GEL (http://www.imm.dtu.dk/GEL/)
// BSP tree in GEL originally written by Bent Dalgaard Larsen.
// This file written by Jeppe Revall Frisvad, 2011
// Copyright (c) DTU Informatics 2011

#include <vector>
#include "Ray.h"
#include "AccObj.h"
#include "AABB.h"
#include "TriMesh.h"
#include "BspTree.h"

using namespace std;

namespace
{
  const float f_eps = 1.0e-6f;  
  const float d_eps = 1.0e-12f;
}

BspTree::~BspTree()
{
  delete_node(root);
}

void BspTree::init(const vector<const TriMesh*>& geometry, const std::vector<const Plane*>& scene_planes)
{
  root = new BspNode;
  Accelerator::init(geometry, scene_planes);
  for(unsigned int i = 0; i < geometry.size(); ++i)
    bbox.add_AABB(geometry[i]->compute_bbox());
  vector<AccObj*> objects = primitives;
  subdivide_node(*root, bbox, 0, objects);
}

bool BspTree::closest_hit(Ray& r) const
{
  closest_plane(r);
  intersect_node(r, *root);
  if(r.has_hit)
    r.hit_pos = r.origin + r.dist*r.direction;  
  return r.has_hit;
}

bool BspTree::any_hit(Ray& r) const
{
  if(any_plane(r))
    return true;
  else
    return intersect_node(r, *root);
}

void BspTree::subdivide_node(BspNode& node, AABB& bbox, unsigned int level, vector<AccObj*>& objects) 
{
  const int TESTS = 4;
  
  if(objects.size() <= max_objects || level == max_level) 
  {
    node.axis_leaf = bsp_leaf; // Means that this is a leaf
    node.id = tree_objects.size();
    node.count = objects.size();
    
    tree_objects.resize(tree_objects.size() + objects.size());
    for(unsigned int i = 0; i < objects.size(); ++i) 
      tree_objects[node.id + i] = objects[i];
  } 
  else 
  {
    bool right_zero=false;
    bool left_zero=false;
    unsigned int i;
    BspNode* left_node  = new BspNode;
    BspNode* right_node = new BspNode;
    vector<AccObj*> left_objects;
    vector<AccObj*> right_objects;
    
    node.left = left_node;
    node.right = right_node;

    int new_axis = -1;
    double min_cost = 1.0e27;
    int new_pos = -1;      

    for(i = 0; i < 3; ++i) 
    {
      for(int k = 1; k < TESTS; ++k) 
      {
        AABB left_bbox = bbox;
        AABB right_bbox = bbox;
                  
        float max_corner = bbox.p_max[i];
        float min_corner = bbox.p_min[i];
        float center = (max_corner - min_corner)*k/static_cast<float>(TESTS) + min_corner;
        node.plane = center;
        
        left_bbox.p_max[i] = center; 
        right_bbox.p_min[i] = center; 

        // Try putting the triangles in the left and right boxes
        int left_count = 0;
        int right_count = 0;
        for(unsigned int j = 0; j < objects.size(); ++j) 
        {
          AccObj* obj = objects[j];
          left_count += left_bbox.intersects(obj->bbox);
          right_count += right_bbox.intersects(obj->bbox);
        }

        double cost = left_count*left_bbox.area() + right_count*right_bbox.area();
        if(cost < min_cost) 
        {
          min_cost = cost;
          new_axis = i;
          new_pos = k;
          right_zero = (right_count == 0);
          left_zero = (left_count == 0);
        }
      }
    }
    node.axis_leaf = static_cast<BspNodeType>(new_axis);
    left_node->axis_leaf = static_cast<BspNodeType>(-1); 
    right_node->axis_leaf = static_cast<BspNodeType>(-1); 

    // Now chose the right splitting plane
    AABB left_bbox = bbox;
    AABB right_bbox = bbox;

    float max_corner = bbox.p_max[node.axis_leaf];
    float min_corner = bbox.p_min[node.axis_leaf];
    float size = max_corner - min_corner;
    float center = size*new_pos/static_cast<float>(TESTS) + min_corner;
    float diff = f_eps < size/8.0f ? size/8.0f : f_eps;
    
    if(left_zero) 
    {
      // Find min position of all triangle vertices and place the center there
      center = max_corner;
      for(unsigned int j = 0; j < objects.size(); ++j) 
      {
        AccObj* obj = objects[j];
        float obj_min_corner = obj->bbox.p_min[node.axis_leaf];
        if(obj_min_corner < center)
          center = obj_min_corner;
      }
      center -= diff;
    }
    if(right_zero) 
    {
      // Find max position of all triangle vertices and place the center there
      center = min_corner;
      for(unsigned int j=0;j<objects.size();j++) 
      {
        AccObj* obj = objects[j];
        float obj_max_corner = obj->bbox.p_max[node.axis_leaf];
        if (obj_max_corner > center)
          center = obj_max_corner;
      }
      center += diff;
    }

    node.plane = center;
    left_bbox.p_max[node.axis_leaf] = center; 
    right_bbox.p_min[node.axis_leaf] = center; 
          
    // Now put the triangles in the right and left node
    for(i = 0; i < objects.size(); ++i) 
    {
      AccObj* obj = objects[i];
      if(left_bbox.intersects(obj->bbox)) 
        left_objects.push_back(obj);
      if(right_bbox.intersects(obj->bbox)) 
        right_objects.push_back(obj);
    }
  //if (left_zero||right_zero)
  //  cout << left_objects.size() << "," << right_objects.size() << "," << level << endl;

    objects.clear();
    subdivide_node(*left_node, left_bbox, level + 1, left_objects);
    subdivide_node(*right_node, right_bbox, level + 1, right_objects);
  }
}

bool BspTree::intersect_node(Ray& ray, const BspNode& node) const 
{
  if(node.axis_leaf == bsp_leaf) 
  {
    bool found = false; 
    for(unsigned int i = 0; i < node.count; ++i) 
    {
      const AccObj* obj = tree_objects[node.id + i];
      if(obj->geometry->intersect(ray, obj->prim_idx))
      {
        ray.tmax = ray.dist;
        found = true;
      }
    }
    return found;
  } 
  else 
  {
    BspNode *near_node;
    BspNode *far_node;
    float axis_direction = ray.direction[node.axis_leaf];
    float axis_origin = ray.origin[node.axis_leaf];
    if(axis_direction >= 0.0f) 
    {
      near_node = node.left;
      far_node = node.right;
    } 
    else 
    {
      near_node = node.right;
      far_node = node.left;
    }

    // In order to avoid instability
    float t;
    if(fabs(axis_direction) < d_eps)
      t = (node.plane - axis_origin)/d_eps; // intersect node plane;
    else
      t = (node.plane - axis_origin)/axis_direction; // intersect node plane;
    
    if(t > ray.tmax) 
      return intersect_node(ray, *near_node);      
    else if(t < ray.tmin) 
      return intersect_node(ray, *far_node);
    else 
    {
      float t_max = ray.tmax;
      ray.tmax = t;
      if(intersect_node(ray, *near_node))
        return true;
      else
      {
        ray.tmin = t;
        ray.tmax = t_max;
        return intersect_node(ray, *far_node);
      }
    }
  }
}

void BspTree::delete_node(BspNode *node) 
{
  if(node)
  {
    if(node->left)
      delete_node(node->left);
    if(node->right)
      delete_node(node->right);
    delete node;
  }
}
