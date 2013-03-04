// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2010
// Copyright (c) DTU Informatics 2010

#ifndef TEXTURED_H
#define TEXTURED_H

#include <map>
#include <string>
#include "CGLA/Vec3f.h"
#include "Ray.h"
#include "Texture.h"
#include "Reflectance.h"

class Textured : public Reflectance
{
public:
  Textured() : texs(0) { }

  void set_textures(std::map<std::string, Texture*>& textures) { texs = &textures; }

protected:
  virtual CGLA::Vec3f get_diffuse(const Ray& r) const;

  std::map<std::string, Texture*>* texs;
};

#endif // TEXTURED_H
