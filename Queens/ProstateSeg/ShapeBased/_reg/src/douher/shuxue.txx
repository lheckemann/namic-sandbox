#ifndef _shuxue_txx_
#define _shuxue_txx_

#include "shuxue.h"

#include <iostream>
#include <vector>
#include <cmath>

#include <vnl/vnl_matlab_filewrite.h>

namespace douher
{
  template< typename T > 
  inline T heaviside(T t, double epsilon)
  {
    return 0.5 + atan(t/epsilon)/(vnl_math::pi);
  }

  template< typename T > 
  inline T delta(T t, double epsilon)
  {
    return epsilon*epsilon/(vnl_math::pi)/(epsilon*epsilon + t*t);
  }


  template< typename T > 
  vnl_vector< T > *pad_zero(const vnl_vector< T > &f, long newSize)
  {
    long oldSize = f.size();
    if (newSize < oldSize)
      {
        std::cerr<<"newSize should be >= oldSize. exit. \n";
        exit(-1);
      }
    
    vnl_vector< T > *newVct = new vnl_vector< T >(newSize, 0.0);

    for (long i = 0; i < oldSize; ++i)
      {
        (*newVct)[i] = f[i];
      }

    return newVct;
  }

  template< typename T > 
  vnl_vector< T > *pad_const(const vnl_vector< T > &f, long newSize)
  {
    long oldSize = f.size();
    if (newSize < oldSize)
      {
        std::cerr<<"newSize should be >= oldSize. exit. \n";
        exit(-1);
      }

    T lastVal = f[oldSize-1];    
    vnl_vector< T > *newVct = new vnl_vector< T >(newSize, lastVal);

    for (long i = 0; i < oldSize; ++i)
      {
        (*newVct)[i] = f[i];
      }

    return newVct;
  }


  template< typename T > 
  vnl_vector< T > *padToPowerOf2_zero(const vnl_vector< T > &f)
  {
    long n = f.size();
    if ( n&&!(n&(n-1)) ) // if n is already power of 2, just return a copy of it
      {
        vnl_vector< T > *padded = new vnl_vector< T >(f);
        return padded;
      }

    double b = log( (double)n )/log(2.0); // log base 2
    double intPartOfb = 0;
    modf(b, &intPartOfb);

    long newSize = (long)pow(2, intPartOfb+1);
    return pad_zero(f, newSize);
  }

  template< typename T > 
  vnl_vector< T > *padToPowerOf2_const(const vnl_vector< T > &f)
  {
    long n = f.size();
    if ( n&&!(n&(n-1)) ) // if n is already power of 2, just return a copy of it
      {
        vnl_vector< T > *padded = new vnl_vector< T >(f);
        return padded;
      }

    double b = log( (double)n )/log(2.0); // log base 2
    double intPartOfb = 0;
    modf(b, &intPartOfb);

    long newSize = (long)pow(2, intPartOfb+1);
    return pad_const(f, newSize);
  }


  /*----------------------------------------
    linspace   */
  //  VnlDoubleVectorType *linspace(double firstVal, double lastVal, long numOfVal)
  template< typename T > 
  vnl_vector< double > *linspace(double firstVal, double lastVal, long numOfVal)
  {
    // We exclude numOfVal = 1 case.
    if (numOfVal <= 1)
      {
        std::cerr<<"numOfVal <= 1, exit. \n";
        exit(-1);
      }

    vnl_vector< double > *f = new vnl_vector< double >(numOfVal);
    f->fill(firstVal);

    double stepSize = (lastVal - firstVal)/(double)(numOfVal - 1);
    for (long ii = 1; ii < numOfVal; ++ii)
      {
        (*f)[ii] = (*f)[ii - 1] + stepSize;
      }
    

    return f;
  }



} // end of namespace

#endif


