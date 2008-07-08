//
// C++ Interface: sbiaSymRealSphericalHarmonicRep
//
// Description: 
//
//
// Author:  <>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef _sbiaSymRealSphericalHarmonicRep_txx
#define _sbiaSymRealSphericalHarmonicRep_txx

#define PI 3.14159265358979323846264338327950288419716939937510

#include "sbiaSymRealSphericalHarmonicRep.h"
//#include "itkNumericTraitsTensorPixel.h"
#include "vnl/vnl_vector.h"
#include "vnl/vnl_vector_fixed.h"

namespace itk
{

/**
 * Assignment Operator
 */
template<class T,unsigned int NDimension>
SymRealSphericalHarmonicRep<T,NDimension>&
SymRealSphericalHarmonicRep<T,NDimension>
::operator= (const Self& r)
{
  BaseArray::operator=(r);
  return *this;
}


/**
 * Assignment Operator from a scalar constant
 */
template<class T,unsigned int NDimension>
SymRealSphericalHarmonicRep<T,NDimension>&
SymRealSphericalHarmonicRep<T,NDimension>
::operator= (const ComponentType & r)
{
  BaseArray::operator=(&r);
  return *this;
}

/**
 * Assigment from a plain array
 */
template<class T,unsigned int NDimension>
SymRealSphericalHarmonicRep<T,NDimension>&
SymRealSphericalHarmonicRep<T,NDimension>
::operator= (const ComponentArrayType r )
{
  BaseArray::operator=(r);
  return *this;
}

/**
 * Assigment from a vnl_vector
 */
template<class T,unsigned int NDimension>
    SymRealSphericalHarmonicRep<T,NDimension>&
        SymRealSphericalHarmonicRep<T,NDimension>
  ::operator= (const vnl_vector<ComponentType> r )
{
  ///check the size...
  if (r.size() != InternalDimension)
  {
    itkExceptionMacro( << "Assignment must assign every element of array:" 
        << "Expecting " << InternalDimension << "elements. But only received " << r.size()
                     );
  }
  for (unsigned int i=0;i<r.size();i++){
    (*this)[i] = r.get(i);
  }
  return *this;
}
  
/**
 * Returns a temporary copy of a vector
 */
template<class T,unsigned int NDimension>
SymRealSphericalHarmonicRep<T,NDimension> 
SymRealSphericalHarmonicRep<T,NDimension>
::operator+(const Self & r) const
{
  Self result;
  for( unsigned int i=0; i<InternalDimension; i++) 
    {
    result[i] = (*this)[i] + r[i];
    }
  return result;
}

/**
 * Returns a temporary copy of a vector
 */
template<class T,unsigned int NDimension>
SymRealSphericalHarmonicRep<T,NDimension> 
SymRealSphericalHarmonicRep<T,NDimension>
::operator-(const Self & r) const
{
  Self result;
  for( unsigned int i=0; i<InternalDimension; i++) 
    {
    result[i] = (*this)[i] - r[i];
    }
  return result;
}
 
/**
 * Performs addition in place
 */
template<class T,unsigned int NDimension>
const SymRealSphericalHarmonicRep<T,NDimension> & 
SymRealSphericalHarmonicRep<T,NDimension>
::operator+=(const Self & r) 
{
  for( unsigned int i=0; i<InternalDimension; i++) 
    {
    (*this)[i] += r[i];
    }
  return *this;
}
 
/**
 * Performs subtraction in place 
 */
template<class T,unsigned int NDimension>
const SymRealSphericalHarmonicRep<T,NDimension> & 
SymRealSphericalHarmonicRep<T,NDimension>
::operator-=(const Self & r)
{
  for( unsigned int i=0; i<InternalDimension; i++) 
    {
    (*this)[i] -= r[i];
    }
  return *this;
}
 
/**
 * Performs multiplication by a scalar, in place
 */
template<class T,unsigned int NDimension>
const SymRealSphericalHarmonicRep<T,NDimension> & 
SymRealSphericalHarmonicRep<T,NDimension>
::operator*=(const RealValueType & r) 
{
  for( unsigned int i=0; i<InternalDimension; i++) 
    {
    (*this)[i] *= r;
    }
  return *this;
}
 
/**
 * Performs division by a scalar, in place
 */
template<class T,unsigned int NOrder>
const SymRealSphericalHarmonicRep<T,NOrder> & 
SymRealSphericalHarmonicRep<T,NOrder>
::operator/=(const RealValueType & r) 
{
  for( unsigned int i=0; i<InternalDimension; i++) 
    {
    (*this)[i] /= r;
    }
  return *this;
}

/**
 * Performs multiplication with a scalar
 */
template<class T,unsigned int NOrder>
SymRealSphericalHarmonicRep<T,NOrder> 
SymRealSphericalHarmonicRep<T,NOrder>
::operator*(const RealValueType & r) const
{
  Self result;
  for( unsigned int i=0; i<InternalDimension; i++) 
    {
    result[i] = (*this)[i] * r;
    }
  return result;
}


/**
 * Performs division by a scalar
 */
template<class T,unsigned int NOrder>
SymRealSphericalHarmonicRep<T,NOrder> 
SymRealSphericalHarmonicRep<T,NOrder>
::operator/(const RealValueType & r) const
{
  Self result;
  for( unsigned int i=0; i<InternalDimension; i++) 
    {
    result[i] = (*this)[i] / r;
    }
  return result;
}

/**
 * Print content to an ostream
 */
template<class T,unsigned int NOrder>
std::ostream &
operator<<(std::ostream& os,const SymRealSphericalHarmonicRep<T,NOrder> & c ) 
{
  for(unsigned int i=0; i<c.GetNumberOfComponents(); i++)
    {
    os <<  static_cast<typename NumericTraits<T>::PrintType>(c[i]) << "  ";
    }
  return os;
}

/**
 * Read content from an istream
 */
template<class T,unsigned int NOrder>
std::istream &
operator>>(std::istream& is, SymRealSphericalHarmonicRep<T,NOrder> & dt ) 
{
  for(unsigned int i=0; i < dt.GetNumberOfComponents(); i++)
    {
    is >> dt[i];
    }
  return is;
}

/**
 * Compute the legendre polynomial. p(l,m,x)
 */
template<class T,unsigned int NOrder>
typename SymRealSphericalHarmonicRep<T,NOrder>::RealValueType
SymRealSphericalHarmonicRep< T,NOrder >
::P( int l, int m, RealValueType x )
{
  // Code taken from 'Robin Green - Spherical Harmonic Lighting'.
  RealValueType pmm = 1;
  if( m > 0 )
  {
    RealValueType somx2 = sqrt( (1-x)*(1+x) );
    RealValueType fact = 1;
    for( int i=1; i<=m; ++i )
    {
      pmm *= -fact * somx2;
      fact += 2;
    }
  }

  if( l == m )
    return pmm;

  RealValueType pmmp1 = x * (2*m+1) * pmm;
  if( l == m+1 )
    return pmmp1;

  RealValueType pll = 0.0;
  for( int ll=m+2; ll<=l; ++ll )
  {
    pll = ( (2*ll-1) * x * pmmp1 - (ll+m-1) * pmm ) / (ll-m);
    pmm = pmmp1;
    pmmp1 = pll;
  }

  return pll;
}

/**
 * Nomalization factor for the spherical harmonics...
 */
template<class T,unsigned int NOrder>
typename SymRealSphericalHarmonicRep<T,NOrder>::RealValueType
SymRealSphericalHarmonicRep< T,NOrder >
::K( int l, int m )
{
  return sqrt( ( (2*l+1) * factorial(l-m) ) / ( 4*( PI ) * factorial(l+m) ) );
}
/**
 * Compute the real spherical harmonics the traditional way.
 * Not sure how this stacks up agains other computation stratagies.
 * TODO THIS NEEDS TO BE CHECKED AGAINST SOPHIA
 * Has been checked against mathematica, ie the using im and re...
 */
template<class T,unsigned int NOrder>
typename SymRealSphericalHarmonicRep<T,NOrder>::RealValueType
SymRealSphericalHarmonicRep< T,NOrder >
::Y( int l, int m, RealValueType theta, RealValueType phi )
{
  const RealValueType sqrt2 = sqrt( static_cast< RealValueType >(2.0) );
  if( m == 0 ) /// Y_l^0
    return K(l,0) * P(l,m,cos(theta));
  else if( m < 0 ) /// sqrt2 im(y_l^m)
    return pow(-1.0,m) * sqrt2 * K(l,-m) * cos(-m*phi) * P(l,-m,cos(theta));
  else ///(m > 0) sqrt2 re(y_l^m)
    return sqrt2 * K(l,m) * sin(m*phi) * P(l,m,cos(theta));
}

/**
 * Compute the factorial
 */
template<class T,unsigned int NOrder>
int
SymRealSphericalHarmonicRep< T,NOrder >
::factorial( int n )
{
  int result = 1;

  for( int i=2; i<=n; ++i )
    result *= i;

  return result;
}
    
/**
 * Evaluate the RealSphericalHarmonicRep as a function of theta and phi.
 */
template<class T,unsigned int NOrder>
typename SymRealSphericalHarmonicRep<T,NOrder>::RealValueType
SymRealSphericalHarmonicRep< T,NOrder >
  ::Evaluate( RealValueType theta, RealValueType phi )
{
  RealValueType result = 0;
  LmVector lm;
  
  for( unsigned int i=0; i<InternalDimension; i++) 
  {
  lm = getLM(i+1);
  result += (*this)[i] * Y(lm[0],lm[1],theta,phi);
  }
  return result;
}

template<class T,unsigned int NOrder>
typename SymRealSphericalHarmonicRep<T,NOrder>::RealValueType
SymRealSphericalHarmonicRep< T,NOrder >
  ::Evaluate(GradientDirectionType Gradient)
{
  double theta = acos(Gradient[2]);
  double phi   = atan2(Gradient[1],Gradient[0]); // atan2(y,x) = atan(y/x);
  
  return (*this).Evaluate(theta,phi);
}
} // end namespace itk

#endif
