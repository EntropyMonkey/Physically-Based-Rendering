// Code from GEL (http://www.imm.dtu.dk/GEL/)
// Modified by Jeppe Revall Frisvad to suit render framework.
// Copyright (c) DTU Informatics 2012

#ifndef INDEXEDFACESET_H
#define INDEXEDFACESET_H

#include <vector>
#include "CGLA/Vec3f.h"
#include "CGLA/Vec3i.h"

/** \brief This class represents the simplest possible triangle mesh data
		structure.

		Faces must be triangular, and 3D coordinates are the only values 
		associated with the vertices. 
    
    User is responsible for checking index bounds. */
class IndexedFaceSet
{
	/// Vertices
  std::vector<CGLA::Vec3f> verts;
		
	/// Vec3is (which must be triangles)
  std::vector<CGLA::Vec3i> faces;
			
public:

	IndexedFaceSet() : verts(0), faces(0) { }

	// ----------------------------------------
	// Functions that operate on faces
	// ----------------------------------------
			
	/** Add a face and return the index of the new face. 
			
	If the optional idx argument is present, the face array is resized so 
	that the new index == idx. */
	unsigned int add_face(const CGLA::Vec3i& f, int idx = -1) 
	{
    unsigned int size = faces.size();
    if(idx < 0)
    {
      faces.push_back(f);
      return size;
    }
    else if(idx > static_cast<int>(size) - 1)
      faces.resize(idx + 1, CGLA::Vec3i(0));

    faces[idx] = f;
    return faces.size() - 1;
  }

	/// Return the number of faces.
	unsigned int no_faces() const { return faces.size(); }

	/// Return the face corresponding to a given index. 
  const CGLA::Vec3i& face(unsigned int idx) const { return faces[idx]; }

	/// Assign f to a face of index idx
	CGLA::Vec3i& face_rw(unsigned int idx) 
	{
		return faces[idx];
	}

	// ----------------------------------------
	// Functions that operate on vertices
	// ----------------------------------------

	/// Add a vertex and return the index of the vertex.
	unsigned int add_vertex(const CGLA::Vec3f& v)
	{
		unsigned int idx = verts.size();
		verts.push_back(v);
		return idx;
	}

	/// Return the number of vertices.
	unsigned int no_vertices() const { return verts.size(); }

	/// Return the vertex corresponding to a given index. 
	const CGLA::Vec3f& vertex(unsigned int idx) const
	{
		return verts[idx];
	}

	/// Assign v to a vertex of index idx
	CGLA::Vec3f& vertex_rw(unsigned int idx)
	{
		return verts[idx];
	}
};

#endif // INDEXEDFACESET_H
