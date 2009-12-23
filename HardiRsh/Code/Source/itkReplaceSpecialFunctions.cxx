
#ifndef __itkReplaceSpecialFunctions_cxx
#define __itkReplaceSpecialFunctions_cxx

#include "itkReplaceSpecialFunctions.h"
#include "vnl/vnl_math.h"
#include <cmath>
#include <cstdlib>

/**
 * Compute the binomial coefficient
 * 
 *    (  n )
 *    (  k )
 *
 * Using Comb6 method described in 
 * Binomial Coefficient Computation: Recursion or Iteration?
 *     Yannis Manolopoulos
 *     manolopo@delab.csd.auth.gr
 */
int
binomialCoeff( int n, int k )
{
  if (k==0 || n==k)
  {
    return 1;
  }

  if (n<0 or k<0 or k > n)
  {
    return 0;
  }

  int result = 1;
  int i;
  if (k < n-k)
  {
    for(i=n; i>= n-k+1; i--)
    {
      result *= i;
      result /= (n-i+1);
    }
  }
  else
  {
    for(i=n; i>= k+1; i--)
    {
      result *= i;
      result /= (n-i+1);
    }
  }
  return result;
}

/**
 * Compute the legendre polynomial. p(l,m,x)
 * 
 * TODO This code nees to be replaced
 * It is a modified version of the Code from Numerical Recipies in C
 * 
 */
double
LegendreP( int l, int m, double x )
{
  //Handle the case of negative m
  if (m < 0)
  {
    //Determine (-1)^(m)
    int sign = 1;
    if ( vcl_abs(m) % 2 == 1) sign = -1;

    // Compute factorial(l+m) / factorial(l-m) 
    double f = 1.0;
    // m < 0 so (l-m) > (l+m)
    // (l+m)!/(l-m)! = Prod
    for (int i = (l-m); i>(l+m) ; i--)
    {
      f /= i;
    }
    
    return sign * f * LegendreP(l,-m,x);
  }

  double pmm = 1;
  if( m > 0 )
  {
    double somx2 = vcl_sqrt( (1-x)*(1+x) );
    double fact = 1;
    for( int i=1; i<=m; ++i )
    {
      pmm *= -fact * somx2;
      fact += 2;
    }
  }

  if( l == m )
    return pmm;

  double pmmp1 = x * (2*m+1) * pmm;

  if( l == m+1 )
    return pmmp1;

  double pll = 0.0;

  for( int ll=m+2; ll<=l; ++ll )
  {
    pll = ( (2*ll-1) * x * pmmp1 - (ll+m-1) * pmm ) / (ll-m);
    pmm = pmmp1;
    pmmp1 = pll;
  }
  return pll;
}

/** Compute the associated legendre polynomial P_n^m(z)
 * 
 *  Use the following recursion relations from wolfram
 *  [1] (n-m) P_n^m(x) = x(2n-1) P_(n-1)^m(x) - (n+m-1) P_(n-2)^m(x). 
 * 
 *  and initializing it using
 *  [2] P_n^n(x)       = (-1)^n (2n-1)!! (1-x^2)^(n/2) 
 *  [3] P_(n+1)^n(x)   = x (2n+1)P_n^n(x). 
 * 
 *  note that (2n-1)!! is the Product of all odd integers <= (2n-1)
 *    so (2n-1)!! = 2n-1 * 2n-3 * 2n-5 * ... * 5 * 3 * 1
 * 
 * Extend the definition to negative m using the following identity
 *  [4] P_n^m(x)       = (-1)^m((n+m)!)/((n-m)!)  P_n^(-m)(x). 
 * 
 **/
/*
double
LegendreP( int n, int m, double x )
{
  //Handle the case of negative m
  if (m < 0)
  {
    //Determine (-1)^(m)
    int sign = 1;
    if ( vcl_abs(m) % 2 == 1) sign = -1;

    // Compute factorial(n+m) / factorial(n-m) 
    double f = 1.0;
    // m < 0 so (n-m) > (n+m)
    // (n+m)!/(n-m)! = Prod
    for (int i = (n-m); i>(n+m) ; i--)
    {
      f /= i;
    }
    
    return sign * f * LegendreP(n,-m,x);
  }

  return 1;
}
*/
#endif //__itkReplaceSpecialFunctions_cxx
