//----------------------------------------------------------------------
//	
// This file was scanned from Henrik Wann Jensen: Realistic image 
// Sythesis Using Photon Mapping, A K Peters, July 2001, by Marianne 
// Bilsted Pedersen, Anders Kyster & Olav Madsen
//
// It has been slightly modified (October 2002) by Andreas Bærentzen
// in order to use CGLA my Linear Algebra library.
//
// Slightly modified (March 2009) by Jeppe Revall Frisvad to enable
// rendering of the photons as points.
//
// Templatified (November 2010) by Jeppe Revall Frisvad to enable 
// mapping of different types of photons. All code is now in this 
// header file.
//----------------------------------------------------------------------

#ifndef __PHOTONMAP_H__
#define __PHOTONMAP_H__
 
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#ifdef WIN32
#include <malloc.h>
#else
#include <alloca.h>
#endif
#include "GL/glut.h"
#include "CGLA/Vec3f.h"

//This is the photon
//The power is not compressed so the size is 28 bytes
struct Photon	
{
	CGLA::Vec3f pos;					    //photon position
	short plane;						      //splitting plane for kd_tree
	unsigned char theta, phi;			//incoming direction
	CGLA::Vec3f power;       			//photon power (uncompressed)
	unsigned int m_type;
};

// This structure is used only to locate the nearest photons
template<class T>
struct NearestPhotons	
{
	int max;
	int found;
	int got_heap;
	CGLA::Vec3f pos;
	float *dist2;
	const T** index;
};

//The following swap function was a define macro in the cpp-file.
template<class T>
inline void photon_swap(T** ph, int a, int b)	{ T* ph2=ph[a]; ph[a]=ph[b]; ph[b]=ph2; }

//This is the PhotonMap class
template<class T = Photon> 
class PhotonMap	
{
public:
	
  /* This is the constructor for the photon map.
     To create the photon map it is necessary to specify the
     maximum number of photons that will be stored	*/
  PhotonMap( int max_phot )
  {
	  stored_photons = 0;
	  prev_scale = 1;
	  max_photons = max_phot;

	  // Allocates an array for the photons (OM)
	  photons = (T*)malloc( sizeof( T ) * ( max_photons+1 ) );

	  if (photons == 0)	{
		  fprintf(stderr,"Out of memory initializing photon map\n");
		  exit(-1);
	  }

	  bbox_min[0] = bbox_min[1] = bbox_min[2] =  1e8f;
	  bbox_max[0] = bbox_max[1] = bbox_max[2] = -1e8f;

	  // initialize direction conversion tables
	  for (int i=0; i<256; i++)	{
		  double angle = double(i)*(1.0/256.0)*M_PI;
      costheta[i]  = std::cos( angle );
      sintheta[i]  = std::sin( angle );
      cosphi[i]	   = std::cos( 2.0*angle );
      sinphi[i]    = std::sin( 2.0*angle );
	  }
  }

  ~PhotonMap()
  {
    std::free( photons );
  }

  int get_photon_count() {
		return stored_photons;
	}

	int get_max_photon_count() {
		return max_photons;
	}

  /*store puts a Photon into the flat array that will form
	  the final kd-tree.
	  Call this function to store a photon.		*/
  void store(
		const CGLA::Vec3f& power,  // photon power
		const CGLA::Vec3f& pos,    // photon position
		const CGLA::Vec3f& dir)  	 // photon direction
  {
	  if (stored_photons >= max_photons)
		  return;
   
    if(power[0] + power[1] + power[2] < 1.0e-8f)
      return;

	  stored_photons++;
	  T* const node = &photons[stored_photons];

	  for (int i=0; i<3; i++)	
	  {
		  node->pos[i] = pos[i];

		  if (node->pos[i] < bbox_min[i])
			  bbox_min[i] = node->pos[i];
		  if (node->pos[i] > bbox_max[i])
			  bbox_max[i] = node->pos[i];

		  node->power[i] = power[i];
	  }

    int theta = int( std::acos(dir[2])*(256.0/M_PI));
	  if ( theta > 255 )
		  node->theta = 255;
	  else
		  node->theta = (unsigned char)theta;

    int phi = int( std::atan2(dir[1], dir[0])*(256.0/(2.0*M_PI)));
	  if ( phi > 255 )
		  node->phi = 255;
	  else if ( phi < 0 )
		  node->phi = (unsigned char)( phi + 256 );
	  else
		  node->phi = (unsigned char)phi;
  }
    
  /* scale-photon-power is used to scale the power of all
	   photons once they have been emitted from the light
	   source. scale = 1/(*emitted photons).
	   Call this function after each light source is processed.		*/
  void scale_photon_power(
    const float scale )        // l/(number of emitted photons)
  {
    for(int i = prev_scale; i <= stored_photons; ++i)
      photons[i].power *= scale;

    mean_scale = (stored_photons - prev_scale - 1)*scale + (prev_scale - 1)*mean_scale;
    mean_scale /= static_cast<float>(stored_photons);
    prev_scale = stored_photons + 1;
  }

  // balance creates a left-balanced kd-tree from the flat photon array.
  // This function should be called before the photon map
  // is used for rendering.
	void balance(void)           // balance the kd_tree (before use!)
  {
	  if (stored_photons > 1 )		{
		  // allocate two temporary arrays for the balancing procedure
      T** pa1 = (T**)std::malloc(sizeof(T*)*(stored_photons+1));
      T** pa2 = (T**)std::malloc(sizeof(T*)*(stored_photons+1));

		  int i;
		  for (i=0; i<=stored_photons; i++)
			  pa2[i] = &photons[i];  

		  balance_segment( pa1, pa2, 1, 1, stored_photons );
      std::free(pa2);

		  // reorganize balanced kd_tree (make a heap)
		  int d, j=1, foo=1;
		  T foo_photon = photons[j];

		  for ( i=1; i<=stored_photons; i++)	{
			  d=pa1[j]-photons;
			  pa1[j] = 0;
			  if (d != foo)
				  photons[j] = photons[d];
			  else {
				  photons[j] = foo_photon;

				  if (i<stored_photons) {
					  for ( ;foo<=stored_photons; foo++)
						  if (pa1[foo] != 0)
							  break;
					  foo_photon = photons[foo];
					  j = foo;
				  }
				  continue;
			  }
			  j = d;
		  }
		  free(pa1);
	  }
	  half_stored_photons = stored_photons/2-1;
  }
	
  //irradiance_estimate computes an irradiance estimate at a given surface position
	const CGLA::Vec3f irradiance_estimate(
		const CGLA::Vec3f& pos,		          // surface position
		const CGLA::Vec3f& normal,          // surface normal at pos
		const float max_dist,               // max distance to look for photons
		const int nphotons ) const          // number of photons to use
  {
    NearestPhotons<T> np;
    np.dist2 = (float*)alloca( sizeof(float)*(nphotons+1));
    np.index = (const T**)alloca( sizeof(T*)*(nphotons+1));

    np.pos = pos;
    np.max = nphotons;
    np.found = 0;
    np.got_heap = 0;
    np.dist2[0] = max_dist*max_dist;

    // locate_photons finds the nearest photons in the map given the parameters in np
    locate_photons( &np, 1 );

    // sum irradiance from all photons
    CGLA::Vec3f irrad(0.0f);
    for (int i = 1; i <= np.found; ++i)
    {
      const Photon *p = np.index[i];
      // the photon_dir call and following if can be omitted (for speed)
      // if the scene does not have any thin surfaces
      if ( dot(photon_dir(p),normal) > 0)
      {
        irrad += p->power;
      }
    }
    irrad *= 1.0 / ( M_PI * np.dist2[0] );    // estimate of density
    return irrad;
  }

  void locate_photons(
    NearestPhotons<T>* const np,        // np is used to locate the photons
    const int index ) const             // call with index = 1
  {	
	  const T* p = &photons[index];
	  float dist1;

	  if (index < half_stored_photons)	{
		  dist1 = np->pos[ p->plane ] - p->pos[ p->plane ];

		  if (dist1 > 0.0)	{			// if dist1 is positive search right plane 
			  locate_photons( np, 2*index+1 );
			  if ( dist1*dist1 < np->dist2[0] )
				  locate_photons( np, 2*index );
		  }	// end if
		  else {							// dist1 is negative, search left first 
			  locate_photons( np, 2*index );
			  if ( dist1*dist1 < np->dist2[0] )
				  locate_photons( np, 2*index+1 );
		  }	// end else
	  } // end if	
  	
	  // compute squared distance between current photon and np->pos
	  dist1  = p->pos[0] - np->pos[0];
	  float dist2 = dist1*dist1;
	  dist1  = p->pos[1] - np->pos[1];
	  dist2  += dist1*dist1;
	  dist1  = p->pos[2] - np->pos[2];
	  dist2  += dist1*dist1;

	  if ( dist2 < np->dist2[0] )		{	
		  // we found a photon :) Insert it in the candidate list
		  if ( np->found < np->max ) {	
			  // heap is not full; use array
			  np->found++;
			  np->dist2[np->found] = dist2;
			  np->index[np->found] = p;
		  } // end if
		  else	{ 
			  int j,parent;

			  if (np->got_heap==0)		{ 		// Do we need to build the heap?
				  // Build heap
				  float dst2;
				  const T* phot;
				  int half_found = np->found>>1;
				  for ( int k=half_found; k>=1; k--)		{ 
					  parent=k;
					  phot = np->index[k];
					  dst2 = np->dist2[k];
					  while ( parent <= half_found )		{ 
						  j = parent+parent;
						  if ( j < np->found && np->dist2[j] < np->dist2[j+1] )
							  j++;
						  if ( dst2 >= np->dist2[j] )
							  break;
						  np->dist2[parent] = np->dist2[j];
						  np->index[parent] = np->index[j];
						  parent=j;
					  }	// end while
					  np->dist2[parent] = dst2; 
					  np->index[parent] = phot;
				  } // end for
				  np->got_heap = 1;
			  }	// end if 
  		
			  // insert new photon into max heap
			  // delete largest element, insert new, and reorder the heap
			  parent=1;
			  j = 2;
			  while ( j <= np->found )		{ 
				  if ( j < np->found && np->dist2[j] < np->dist2[j+1] )
					  j++;
				  if ( dist2 > np->dist2[j] )
					  break;
				  np->dist2[parent] = np->dist2[j];
				  np->index[parent] = np->index[j];
				  parent = j;
				  j += j;
			  } // end while
        if(dist2 < np->dist2[parent])
        {
			    np->index[parent] = p;
			    np->dist2[parent] = dist2;
        }
			  np->dist2[0] = np->dist2[1];
		  }	// end else
	  }	// end if
  }	// end locate_photons

  // returns the direction of a photon
	const CGLA::Vec3f photon_dir(
		const T* p) const                   // the photon
  {
    CGLA::Vec3f dir;
	  dir[0] = sintheta[p->theta]*cosphi[p->phi];
	  dir[1] = sintheta[p->theta]*sinphi[p->phi];
	  dir[2] = costheta[p->theta];
	  return dir;
  }

  void draw()
  {
    if(!glIsList(disp_list))
    {
      disp_list = glGenLists(1);
      glNewList(disp_list, GL_COMPILE);

      glBegin(GL_POINTS);
      for(int i = 1; i <= stored_photons; ++i)
      {
        CGLA::Vec3f color = photons[i].power/mean_scale;
        glColor3fv(color.get());
        glVertex3fv(photons[i].pos.get());
      }
      glEnd();

      glEndList();
    }

    glCallList(disp_list);
  }

private:

  // See "Realistic Image Synthesis using Photon Mapping" chapter 6 
  // for an explanation of this function
	void balance_segment(
		T** pbal,
		T** porg,
		const int index,
		const int start,
		const int end )
  {
	  // compute new median
	  int median = 1;
	  while ((4*median) <= (end-start+1))
		  median += median;

	  if ((3*median) <= (end-start+1))	{
		  median += median;
		  median += start-1;
	  } 
	  else
		  median = end-median+1;

	  // find axis to split along 
	  int axis=2;
	  if ((bbox_max[0]-bbox_min[0])>(bbox_max[1]-bbox_min[1]) && 
			  (bbox_max[0]-bbox_min[0])>(bbox_max[2]-bbox_min[2]))
		  axis=0;
	  else if ((bbox_max[1]-bbox_min[1])>(bbox_max[2]-bbox_min[2]))
		  axis=1;

	  // partition photon block around the median
	  median_split( porg, start, end, median, axis );

	  pbal[ index ] = porg[ median ];
	  pbal[ index ]->plane = axis;

	  // recursively balance the left and right block
	  if ( median > start )				{
		  // balance left segment
		  if ( start < median-1 )		{
			  const float tmp = bbox_max[axis];
			  bbox_max[axis] = pbal[index]->pos[axis];
			  balance_segment( pbal, porg, 2*index, start, median-1 );
			  bbox_max[axis] = tmp;
		  }
		  else {
			  pbal[2*index] = porg[start];
		  }
	  }

	  if ( median < end )				{
		  // balance right segment
		  if ( median+1 < end )		{
			  const float tmp = bbox_min[axis];
			  bbox_min[axis] = pbal[index]->pos[axis];
			  balance_segment( pbal, porg, 2*index+1, median+1, end );
			  bbox_min[axis] = tmp;
		  }
		  else {
			  pbal[2*index+1] = porg[end];
		  }
	  }
  }

  // median_split splits the photon array into two separate
  // pieces around the median, with all photons below the
  // median in the lower half and all photons above
  // the median in the upper half. The comparison
  // criteria is the axis ( indicated by the axis parameter )
  // ( inspired by routine in Sedgewick "Algoritms in C++ )
	void median_split(
		T** p,
		const int start,
		const int end,
		const int median,
		const int axis )
  {
	  int left = start;
	  int right = end;

	  while ( right > left )			{
		  const float v = p[right]->pos[axis];
		  int i = left-1;
		  int j=right;
		  for (;;) {
			  while ( p[++i]->pos[axis] < v )
				  ;
			  while ( p[--j]->pos[axis] > v && j>left )
				  ;
			  if ( i >= j )
				  break;
			  photon_swap(p,i,j);
		  }

		  photon_swap(p,i,right);
		  if ( i >= median )
			  right = i-1;
		  if ( i <= median )
			  left=i+1;
	  }
  }

protected:
	T* photons;

	int stored_photons;
	int half_stored_photons;
	int max_photons;
	int prev_scale;

	float costheta[256];
	float sintheta[256];
	float cosphi[256];
	float sinphi[256];

	CGLA::Vec3f bbox_min;			 // use bbox_min; 
	CGLA::Vec3f bbox_max;			 // use bbox_max;

  unsigned int disp_list;
  float mean_scale;
};

#endif // __PHOTONMAP_H__
