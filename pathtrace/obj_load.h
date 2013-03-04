// Code from GEL (http://www.imm.dtu.dk/GEL/)
// Copyright (c) DTU Informatics 2011

#ifndef OBJ_LOAD_H
#define OBJ_LOAD_H

#include <string>
#include <vector>
#include "ObjMaterial.h"
#include "TriMesh.h"

/// Load a TriMesh from an OBJ file
void obj_load(const std::string &filename, TriMesh &mesh);

/// Load materials from an MTL file
void mtl_load(const std::string& filename, std::vector<ObjMaterial>& materials);

#endif // OBJ_LOAD_H
