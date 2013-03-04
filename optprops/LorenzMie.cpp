/*
 * The following source code implements Lorenz-Mie theory
 * using the formulas presented in the SIGGRAPH 2007 paper
 *
 * Computing the Scattering Properties of Participating
 * Media Using Lorenz-Mie Theory
 *
 * By authors: Jeppe Revall Frisvad
 *             Niels Joergen Christensen
 *             Henrik Wann Jensen
 *
 * The functions computing the different coefficients in 
 * the theory are named using the names of the symbols
 * in the paper. The only exception is that the ratio
 * between psi_n and zeta_n is called R_n in the code.
 *
 * Code written by Jeppe Revall Frisvad, 2006.
 * Copyright (c) ACM 2007
 *
 * The ACM copyright covers publication of the code in 
 * whole or in part in any and all forms of media.
 * 
 * Permission is granted to anyone to use this code as 
 * software for any purpose, including commercial applications.
 * However, the software is provided 'as-is' without any warranty.
 */ 

#include <iostream>
#include <complex>
#include <cmath>
#include <valarray>

#include "LorenzMie.h"

using namespace std;

//
// Variables and functions private to this file
//
namespace
{
  unsigned int M = 0;

  valarray< complex<double> > particle_A;
  valarray< complex<double> > host_A;

  unsigned int current_n = static_cast<unsigned int>(-1);
  complex<double> a, b;

  void A_all_n(valarray< complex<double> >& A, 
	       const complex<double>& z, 
	       unsigned int userM = 0)
  {
    if(userM > M) M = userM;
    
    A.resize(M + 2);  

    A[M + 1] = complex<double>(0.0, 0.0);

    if(M == 0) return;
    
    for(unsigned int n = M; n <= M; --n)
    {
      complex<double> tmp = (n + 1.0)/z;
      A[n] = tmp - 1.0/(tmp + A[n + 1]);
    }
  }

  complex<double> psi_zeta(unsigned int n, 
			   const complex<double>& z, 
			   const complex<double>& old_B,
			   const valarray< complex<double> >& A)
  {
    static unsigned int old_n = 0;
    static complex<double> old_psi_zeta;

    if(old_n == 0)
      old_psi_zeta = 0.5*(1.0 - exp(complex<double>(-2.0*z.imag(), 
						    2.0*z.real())));

    if(n == old_n + 1)
    {
      complex<double> n_z = static_cast<double>(n)/z;
      old_psi_zeta = old_psi_zeta*(n_z - A[n - 1])*(n_z - old_B);
      if(n == M)
	old_n = 0;
      else
	old_n = n;
    }
    else if(n != old_n && n != M)
      cerr << "error in computation of psi_n*zeta_n" << endl;

    return old_psi_zeta;
  }

  complex<double> B(unsigned int n, 
		    const complex<double>& z, 
		    valarray< complex<double> >& A)
  {
    static unsigned int old_n = 0;
    static complex<double> old_B;

    if(old_n == 0)
      old_B = complex<double>(0.0, 1.0);

    if(n > A.size()) A_all_n(A, z, n);

    if(n == old_n + 1)
    {
      old_B = A[n] + complex<double>(0.0, 1.0)/psi_zeta(n, z, old_B, A);
      if(n == M)
	old_n = 0;
      else
	old_n = n;
    }
    else if(n != old_n && n != M)
      cerr << "error in computation of B_n" << endl;

    return old_B;
  }

  complex<double> R(unsigned int n, 
		    complex<double> z, 
		    valarray< complex<double> >& A)
  {
    static unsigned int old_n = 0;
    static complex<double> old_R;

    if(old_n == 0)
      old_R = 0.5*(1.0 - exp(complex<double>(2.0*z.imag(), 
					     -2.0*z.real())));

    if(n > A.size()) A_all_n(A, z, n);

    if(n == old_n + 1)
    {
      complex<double> n_z = static_cast<double>(n)/z;
      old_R = old_R*(B(n, z, A) + n_z)/(A[n] + n_z);
      if(n == M)
	old_n = 0;
      else
	old_n = n;
    }
    else
      cerr << "error in computation of R_n" << endl;

    return old_R;
  }

  void LorenzMie_ab(unsigned int n, double size, 
		    const std::complex<double>& host_idx,
		    const std::complex<double>& particle_idx)
  {
    static unsigned int old_n = 0;

    if(n == old_n) 
      return;
    if(n != old_n + 1)
    {
      cerr << "Error: The Lorenz-Mie coefficients must be "
	   << "computed consecutively starting from n=1 and "
	   << "counting upwards to n=terms_to_sum(host_idx*size)." 
	   << endl;
      exit(0);
    }
    if(n == M)
      old_n = 0;
    else
      old_n = n;

    complex<double> particle_z = particle_idx*size;
    complex<double> host_z = host_idx*size;
  
    complex<double> B_n = B(n, host_z, host_A);
    complex<double> R_n = R(n, host_z, host_A);

    a = R_n*(host_idx*particle_A[n] - particle_idx*host_A[n])
            /(host_idx*particle_A[n] - particle_idx*B_n);
    b = R_n*(particle_idx*particle_A[n] - host_idx*host_A[n])
            /(particle_idx*particle_A[n] - host_idx*B_n);
  }

  inline unsigned int terms_to_sum(const complex<double>& z)
  { 
    double size = abs(z);
    M = static_cast<unsigned int>(ceil(size + 4.3*pow(size, 1.0/3.0) + 1.0));
    return M;
  }
}

//
// P U B L I C   F U N C T I O N S
//
namespace LorenzMie
{
  void particle_props(double& C_t, double& C_s, double& C_a, 
		      double& g, double& ior,
		      double radius, double wavelength, 
		      const std::complex<double>& host_refrac,
		      const std::complex<double>& particle_refrac)
  {
#ifndef M_PI
    static const double M_PI = 3.14159265358979323846;
#endif
    double size = 2.0*M_PI*radius/wavelength;
    
    M = terms_to_sum(host_refrac*size);
    A_all_n(particle_A, particle_refrac*size);
    A_all_n(host_A, host_refrac*size);  

    C_s = 0.0;    
    g = 0.0;
    complex<double> S0_k(0.0, 0.0);
    
    LorenzMie_ab(1, size, host_refrac, particle_refrac);
    
    complex<double> host_refrac_sqr = host_refrac*host_refrac;
    for(unsigned int n = 1; n < M; ++n)
    {      
      complex<double> a_n = a;
      complex<double> b_n = b;

      double tmp = 2.0*n + 1.0;
      S0_k += tmp*(a_n + b_n)/host_refrac_sqr;        
      C_s += tmp*(norm(a_n) + norm(b_n));    // for z = x + iy: norm(z) = x^2 + y^2 

      LorenzMie_ab(n + 1, size, host_refrac, particle_refrac);
      
      g += (n*(n + 2.0))/(n + 1.0)*((a_n*conj(a)).real() + (b_n*conj(b)).real())
  	   + tmp/(n*(n + 1.0))*(a_n*conj(b_n)).real();
    }

    double scale = wavelength*wavelength/(2.0*M_PI);    
    double tmp = 2.0*M + 1.0;
    S0_k += tmp*(a + b)/host_refrac_sqr;
    C_t = scale*real(S0_k);
    C_s += tmp*(norm(a) + norm(b)); 
    g *= 2.0/C_s;
    double alpha = 2.0*size*host_refrac.imag();
    C_s *= scale*exp(-alpha)*(alpha > 1.0e-6 
    			      ? alpha*alpha/(2.0*(1.0 + (alpha - 1.0)*exp(alpha)))
    			      : 1.0);
    C_s /= norm(host_refrac);
    C_a = C_t - C_s;
    ior = scale*imag(S0_k);
  }

  void optical_props(ParticleDistrib* p, 
                     double wavelength, 
                     const complex<double>& host_refrac,
                     const complex<double>* particle_refrac)
  {
    if(!p)
    {
      cerr << "Error: Particle distribution p is a null pointer.";
      return;
    }
    if(p->N.size() < static_cast<unsigned int>((p->r_max - p->r_min)/(p->dr - p->dr*1.0e-8)))
    {
      cerr << "Error: The size of p->N does not match the step size p->dr .";
      return;
    }

    p->ext = 0.0;
    p->sca = 0.0;
    p->abs = 0.0;
    p->g = 0.0;
    p->ior = 0.0;
    int counter = 0;
    for(double r = p->r_min + p->dr*0.5; r < p->r_max; r += p->dr)
    {
      double C_t, C_s, C_a, g, ior;
      particle_props(C_t, C_s, C_a, g, ior, 
                     r, wavelength,
                     host_refrac,
                     particle_refrac ? *particle_refrac : p->refrac_idx);
      
      double sigma_s = C_s*p->N[counter]*p->dr;
      p->ext += C_t*p->N[counter]*p->dr;
      p->sca += sigma_s;
      p->abs += C_a*p->N[counter]*p->dr;
      p->g += g*sigma_s;
      p->ior += ior*p->N[counter]*p->dr;
      ++counter;
    }
    if(p->sca > 0.0)
      p->g /= p->sca;
  }
} // LorenzMie

