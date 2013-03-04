// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef RADIANCETRANSFER_H
#define RADIANCETRANSFER_H

#include <fstream>
#include <valarray>
#include <map>
#include "CGLA/Vec3f.h"
#include "TriMesh.h"
#include "SHLight.h"
#include "Shader.h"

class RadianceTransfer : public Shader
{
public:
  RadianceTransfer(const char* prt_filename, const SHLight* sh_light, unsigned int no_of_bands) 
    : fin(prt_filename, std::ios_base::in | std::ios_base::binary), 
      light(sh_light), bands(no_of_bands), idx_counter(0)
  { }

  ~RadianceTransfer() { fin.close(); }

  virtual void init();
  virtual void set_attributes(const TriMesh* mesh, int index);

  void update_light();

protected:
  void load_next_coeffs(const TriMesh* mesh, int index);

  std::ifstream fin;
  const SHLight* light;
  unsigned int bands;
  std::map< int, std::valarray<CGLA::Vec3f> > coeffs;
  const TriMesh* current_mesh;
  int idx_counter;
};

#endif // RADIANCETRANSFER_H
