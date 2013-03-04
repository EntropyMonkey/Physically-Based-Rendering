// Inspired by BSP tree in GEL (http://www.imm.dtu.dk/GEL/)
// BSP tree in GEL originally written by Bent Dalgaard Larsen.
// This file written by Jeppe Revall Frisvad, 2011
// Copyright (c) DTU Informatics 2011

#ifndef BSPTREE_H
#define BSPTREE_H

#include <vector>
#include "Ray.h"
#include "AccObj.h"
#include "TriMesh.h"
#include "AABB.h"
#include "Plane.h"
#include "Accelerator.h"

enum BspNodeType { bsp_x_axis, bsp_y_axis, bsp_z_axis, bsp_leaf };

struct BspNode 
{
  BspNode() : left(0), right(0) { }

  BspNodeType axis_leaf; // 00 = axis 0, 01 = axis 1, 10 = axis 2, 11 = leaf
  float plane;
  BspNode *left, *right;
  unsigned int id;
  unsigned int count;
  unsigned int ref;
};

class BspTree : public Accelerator
{
public:
  BspTree(unsigned int max_objects_in_leaf = 4, unsigned int max_levels_in_tree = 20) 
    : root(0), max_objects(max_objects_in_leaf), max_level(max_levels_in_tree) 
  { }

  virtual ~BspTree();
  virtual void init(const std::vector<const TriMesh*>& geometry, const std::vector<const Plane*>& planes);
  virtual bool closest_hit(Ray& r) const;
  virtual bool any_hit(Ray& r) const;

private:
  void subdivide_node(BspNode& node, AABB& bbox, unsigned int level, std::vector<AccObj*>& objects);
  bool intersect_node(Ray& ray, const BspNode& node) const;
  void delete_node(BspNode *node);

  std::vector<AccObj*> tree_objects;
  BspNode* root;
  AABB bbox;
  unsigned int max_objects;
  unsigned int max_level;
};

#endif // BSPTREE_H