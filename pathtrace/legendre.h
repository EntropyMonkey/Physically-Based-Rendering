// 02566 Rendering Framework
// Written by Jeppe Revall Frisvad, 2009
// Copyright (c) DTU Informatics 2009

#ifndef LEGENDRE_H
#define LEGENDRE_H

#include <valarray>

double factorial(unsigned int n);
double legendre_P(unsigned int ell, unsigned int m, double x);
double sh_normalization(unsigned int ell, unsigned int m);
double spherical_harmonics(unsigned int ell, int m, double cos_theta, double phi);
void spherical_harmonics(unsigned int no_of_bands, double cos_theta, double phi, std::valarray<double>& sh_result);

#endif // LEGENDRE_H
