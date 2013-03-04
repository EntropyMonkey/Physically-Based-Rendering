// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2010
// Copyright (c) DTU Informatics 2010

#ifndef INCANDESCENT_H
#define INCANDESCENT_H

#include "Medium.h"

// Black body emission
Medium blackbody(double temperature); // temperature in Kelvin

// Computing blackbody temperature and emission from light bulb dimensions
// Default is a classic 100 Watts, 2.5 cm^2 tungsten filament, 6 cm diameter light bulb
Medium incandescent(double power = 100.0,           // power in Watts
                    double bulb_area = 1.13e-2,     // bulb surface area in m^2
                    double filament_area = 2.5e-4); // filament surface area in m^2

#endif // INCANDESCENT_H
