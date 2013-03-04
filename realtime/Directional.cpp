// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#include <GL/glew.h>
#include "Directional.h"

void Directional::set_light(GLenum light) const
{
  // Set the properties of the GL light given as argument using the
  // data fields of this directional light class.
  //
  // Input:  light (GL light identifier)
  //
  // Output: Calls to glLightfv
  //
  // Relevant data fields that are available (see Directional.h):
  // light_dir  (direction of the emitted light)
  // emission   (radiance of the emitted light)
  //
  // Hint: The vector datatypes in the framework have a member function
  //       get() such that light_dir.get(), for example, retrieves a
  //       pointer to the data in the vector.
}
