// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef LOAD_MPML
#define LOAD_MPML

#include <map>
#include <string>
#include "Medium.h"
#include "Interface.h"

void load_mpml(std::string filename, 
               std::map<std::string, Medium>& media, 
               std::map<std::string, Interface>& interface_map);

#endif // LOAD_MPML
