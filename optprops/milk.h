// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef MILK_H
#define MILK_H

#include "Medium.h"
#include "LorenzMie.h"

struct LogNormalParticleDistrib : public LorenzMie::ParticleDistrib
{
  double r_vs;  // volume-to-surface mean particle radius
  double c_s;   // particle radius standard deviation
};

Medium milk(LogNormalParticleDistrib& fat,     // Distribution of fat globules
            LogNormalParticleDistrib& casein); // Distribution of casein micelles

Medium milk(LogNormalParticleDistrib& fat,     // Distribution of fat globules
            LogNormalParticleDistrib& casein,  // Distribution of casein micelles
            double fat_weight,                 // milk fat weight-%
            double protein_weight = 3.4);      // protein weight-%

Medium milk(double fat_weight,                 // milk fat weight-%
            double protein_weight = 3.4,       // protein weight-%
            double fat_r_vs = 475.0e-9,        // volume-to-surface mean radius of fat globules
            double fat_c_s = 0.285,            // radius standard deviation of fat globules
            double casein_r_vs = 43.0e-9,      // volume-to-surface mean radius of casein micelles
            double casein_c_s = 0.23);         // radius standard deviation of casein micelles

Medium homogenized_milk(double fat_weight,            // milk fat weight-%
                        double protein_weight = 3.4,  // protein weight-%
                        double pressure = 0.0,        // Homogenization pressure
                        double casein_r_vs = 43.0e-9, // volume-to-surface mean radius of casein micelles
                        double casein_c_s = 0.23);    // radius standard deviation of casein micelles

Medium unhomogenized_milk(double fat_weight,            // milk fat weight-%
                          double protein_weight = 3.4,  // protein weight-%
                          double casein_r_vs = 43.0e-9, // volume-to-surface mean radius of casein micelles
                          double casein_c_s = 0.23);    // radius standard deviation of casein micelles

#endif // MILK_H