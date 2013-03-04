// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef LUMINANCE_H
#define LUMINANCE_H

inline double get_luminance_Avg(double r, double g, double b)
{
  return (r + g + b)/3.0;                  // Average
}

inline double get_luminance_Adobe(double r, double g, double b)
{
  return r*0.2974 + g*0.6273 + b*0.0753;   // Adobe RGB
}

inline double get_luminance_sRGB(double r, double g, double b)
{
  return r*0.2126 + g*0.7152 + b*0.0722;   // sRGB (also HDTV)
}

inline double get_luminance_NTSC(double r, double g, double b)
{
  return r*0.2989 + g*0.5866 + b*0.1145;   // NTSC (1953)
}

inline double get_luminance_SMPTE_C(double r, double g, double b)
{
  return r*0.2124 + g*0.7010 + b*0.0865;   // SMPTE-C (replacing NTSC)
}

inline double get_luminance_PAL(double r, double g, double b)
{
  return r*0.2220 + g*0.7066 + b*0.0713;   // PAL/SECAM
}

inline double get_luminance_Wide(double r, double g, double b)
{
  return r*0.2587 + g*0.7247 + b*0.0166;   // Wide gamut
}

#endif