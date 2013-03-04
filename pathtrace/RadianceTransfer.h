// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef RADIANCETRANSFER_H
#define RADIANCETRANSFER_H

#include <fstream>
#include <vector>
#include "CGLA/Vec3f.h"
#include "Ray.h"
#include "PathTracer.h"
#include "Light.h"
#include "MCGlossy.h"

class RadianceTransfer : public MCGlossy
{
public:
  RadianceTransfer(PathTracer* path_tracer, 
                   const std::vector<Light*>& light_vector, 
                   unsigned int no_of_samples,
                   unsigned int no_of_prt_samples,
                   unsigned int no_of_bands,
                   std::ofstream* prt_file = 0) 
    : MCGlossy(path_tracer, light_vector, no_of_samples), 
      prt_samples(no_of_prt_samples), bands(no_of_bands), fout(prt_file)
  { }

  virtual CGLA::Vec3f shade(Ray& r, bool emit = true) const;

  void set_prt_file(std::ofstream* prt_file) { if(fout) (*fout).flush(); fout = prt_file; }

protected:
  unsigned int prt_samples;
  unsigned int bands;
  std::ofstream* fout;
};

#endif // RADIANCETRANSFER_H
