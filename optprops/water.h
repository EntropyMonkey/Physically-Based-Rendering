// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef WATER_H
#define WATER_H

#include "Medium.h"

// Real part of refractive index is based on Quan and Fry 1995.
// Imaginary part is based on Pope and Fry 1997 and Hale and Query 1973.
// Temperature and salinity correction of imaginary part is based on Pegau et al. 1997.
Medium water(double temperature = 22.0, // Temperature in degrees celcius 
             double salinity = 0.0);    // Salinity in parts per thousand

// Empirical formula of Quan and Fry 1995.
double water_ior_real_part(double wavelength,  // Wavelength in nanometers
                           double temperature, // Temperature in degrees celcius 
                           double salinity);   // Salinity in parts per thousand

#endif // WATER_H