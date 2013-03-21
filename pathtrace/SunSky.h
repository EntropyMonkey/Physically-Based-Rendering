// Based on NVIDIA's OptiX implementation of 2009.

#ifndef SUNSKY_H
#define SUNSKY_H

#include "CGLA/Vec3f.h"
#include "Light.h"

class RayTracer;

//------------------------------------------------------------------------------
//
// Implements the Preetham analytic sun/sky model ( Preetham, SIGGRAPH 99 )
//
//------------------------------------------------------------------------------
class PreethamSunSky : public Light
{
public:
  PreethamSunSky(RayTracer* ray_tracer, unsigned int no_of_samples = 1);
  

  virtual bool sample(const CGLA::Vec3f& pos, CGLA::Vec3f& dir, CGLA::Vec3f& L) const;

  void setSunTheta( float sun_theta )     { _sun_theta = sun_theta; _dirty = true; }
  void setSunPhi( float sun_phi)          { _sun_phi = sun_phi;     _dirty = true; }
  void setTurbidity( float turbidity )    { _turbidity = turbidity; _dirty = true; }

  void setUpDir( const CGLA::Vec3f& up )  { _up = up; _dirty = true; }
  void setOvercast( float overcast )      { _overcast = overcast;    }
  
  float getSunTheta()                     { return _sun_theta; }
  float getSunPhi()                       { return _sun_phi;   }
  float getTurbidity()                    { return _turbidity; }
  
  float getOvercast()                     { return _overcast;              }
  CGLA::Vec3f getUpDir()                  { return _up;                    }
  CGLA::Vec3f getSunDir()                 { init(); return _sun_dir; }

  // Query the sun color at current sun position and air turbidity ( kilo-cd / m^2 )
  CGLA::Vec3f sunColor();

  // Query the sky color in a given direction ( kilo-cd / m^2 )
  CGLA::Vec3f skyColor( const CGLA::Vec3f & direction, bool CEL = false );
  
  // Set precomputed Preetham model variables on the given context:
  //   c[0-4]          : 
  //   inv_divisor_Yxy :
  //   sun_dir         :
  //   sun_color       :
  //   overcast        :
  //   up              :
  void init();

private:
  CGLA::Vec3f calculateSunColor();

  // Represents one entry from table 2 in the paper
  struct Datum  
  {
    float wavelength;
    float sun_spectral_radiance;
    float k_o;
    float k_wa;
  };
  
  static const float cie_table[38][4];          // CIE spectral sensitivy curves
  static const Datum data[38];                  // Table2

  // Calculate absorption for a given wavelength of direct sunlight
  static float calculateAbsorption( float sun_theta, // Sun angle from zenith
                                    float m,         // Optical mass of atmosphere
                                    float lambda,    // light wavelength
                                    float turbidity, // atmospheric turbidity
                                    float k_o,       // atten coeff for ozone
                                    float k_wa );    // atten coeff for h2o vapor
  
  // Unit conversion helpers
  static CGLA::Vec3f XYZ2rgb( const CGLA::Vec3f& xyz );
  static CGLA::Vec3f Yxy2XYZ( const CGLA::Vec3f& Yxy );
  static float rad2deg( float rads );

  // Input parameters
  float  _sun_theta;
  float  _sun_phi;
  float  _turbidity;
  float  _overcast;
  CGLA::Vec3f _up;
  CGLA::Vec3f _sun_color;
  CGLA::Vec3f _sun_dir;

  // Precomputation results
  bool   _dirty;
  CGLA::Vec3f _c0;
  CGLA::Vec3f _c1;
  CGLA::Vec3f _c2;
  CGLA::Vec3f _c3;
  CGLA::Vec3f _c4;
  CGLA::Vec3f _inv_divisor_Yxy;
};



  
inline float PreethamSunSky::rad2deg( float rads )
{
  return rads * 180.0f / static_cast<float>( M_PI );
}


inline CGLA::Vec3f PreethamSunSky::Yxy2XYZ( const CGLA::Vec3f& Yxy )
{
  return CGLA::Vec3f( Yxy[1]*(Yxy[0]/Yxy[2]), Yxy[0], (1.0f - Yxy[1] - Yxy[2])*(Yxy[0]/Yxy[2]) );
}
/*
// sRGB color space
inline CGLA::Vec3f PreethamSunSky::XYZ2rgb( const CGLA::Vec3f& xyz)
{
  const float R = CGLA::dot( xyz, CGLA::Vec3f(  3.2410f, -1.5374f, -0.4986f ) );
  const float G = CGLA::dot( xyz, CGLA::Vec3f( -0.9692f,  1.8760f,  0.0416f ) );
  const float B = CGLA::dot( xyz, CGLA::Vec3f(  0.0556f, -0.2040f,  1.0570f ) );
  return CGLA::Vec3f( R, G, B );
}

// NTSC color space
inline CGLA::Vec3f PreethamSunSky::XYZ2rgb( const CGLA::Vec3f& xyz)
{
  const float R = CGLA::dot( xyz, CGLA::Vec3f(  1.9100f, -0.5325f, -0.2882f ) );
  const float G = CGLA::dot( xyz, CGLA::Vec3f( -0.9847f,  1.9992f, -0.0283f ) );
  const float B = CGLA::dot( xyz, CGLA::Vec3f(  0.0583f, -0.1184f,  0.8976f ) );
  return CGLA::Vec3f( R, G, B );
}
*/
// Wide gamut color space
inline CGLA::Vec3f PreethamSunSky::XYZ2rgb( const CGLA::Vec3f& xyz)
{
  const float R = CGLA::dot( xyz, CGLA::Vec3f(  1.4625f, -0.1845f, -0.2734f ) );
  const float G = CGLA::dot( xyz, CGLA::Vec3f( -0.5228f,  1.4479f,  0.0681f ) );
  const float B = CGLA::dot( xyz, CGLA::Vec3f(  0.0346f, -0.0958f,  1.2875f ) );
  return CGLA::Vec3f( R, G, B );
}

#endif // SUNSKY_H