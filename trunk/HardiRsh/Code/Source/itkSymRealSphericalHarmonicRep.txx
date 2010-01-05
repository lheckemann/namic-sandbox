//
// C++ Interface: itkSymRealSphericalHarmonicRep
//
// Description:
//
//
// Author:  <>, (C) 2008
//
//
//
#ifndef __itkSymRealSphericalHarmonicRep_txx
#define __itkSymRealSphericalHarmonicRep_txx

#include "itkSymRealSphericalHarmonicRep.h"

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
 * Returns a temporary copy of a vector
 */
template<class T,unsigned int NDimension>
SymRealSphericalHarmonicRep<T,NDimension>
SymRealSphericalHarmonicRep<T,NDimension>
::operator+(const Self & r) const
{
  Self result;
  for( unsigned int i=0; i<Dimension; i++)
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
  for( unsigned int i=0; i<Dimension; i++)
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
  for( unsigned int i=0; i<Dimension; i++)
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
  for( unsigned int i=0; i<Dimension; i++)
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
  for( unsigned int i=0; i<Dimension; i++)
    {
    (*this)[i] *= r;
    }
  return *this;
}

/**
 * Performs division by a scalar, in place
 */
template<class T,unsigned int NDimension>
const SymRealSphericalHarmonicRep<T,NDimension> &
SymRealSphericalHarmonicRep<T,NDimension>
::operator/=(const RealValueType & r)
{
  for( unsigned int i=0; i<Dimension; i++)
    {
    (*this)[i] /= r;
    }
  return *this;
}

/**
 * Performs multiplication with a scalar
 */
template<class T,unsigned int NDimension>
SymRealSphericalHarmonicRep<T,NDimension>
SymRealSphericalHarmonicRep<T,NDimension>
::operator*(const RealValueType & r) const
{
  Self result;
  for( unsigned int i=0; i<Dimension; i++)
    {
    result[i] = (*this)[i] * r;
    }
  return result;
}


/**
 * Performs division by a scalar
 */
template<class T,unsigned int NDimension>
SymRealSphericalHarmonicRep<T,NDimension>
SymRealSphericalHarmonicRep<T,NDimension>
::operator/(const RealValueType & r) const
{
  Self result;
  for( unsigned int i=0; i<Dimension; i++)
    {
    result[i] = (*this)[i] / r;
    }
  return result;
}

/**
 * Print content to an ostream
 */
template<class T,unsigned int NDimension>
std::ostream &
operator<<(std::ostream& os,const SymRealSphericalHarmonicRep<T,NDimension> & c )
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
template<class T,unsigned int NDimension>
std::istream &
operator>>(std::istream& is, SymRealSphericalHarmonicRep<T,NDimension> & dt )
{
  for(unsigned int i=0; i < dt.GetNumberOfComponents(); i++)
    {
    is >> dt[i];
    }
  return is;
}

/**
 * Evaluate the RealSphericalHarmonicRep as a function of theta and phi.
 */
template<class T,unsigned int NDimension>
const typename SymRealSphericalHarmonicRep<T,NDimension>::RealValueType
SymRealSphericalHarmonicRep<T,NDimension>
  ::Evaluate( RealValueType theta, RealValueType phi ) const
{
  RealValueType result = 0;
  for( unsigned int i=0; i<Dimension; i++)
  {
    result += (*this)[i] * Y(i+1,theta,phi);
  }
  return result;
}

template<class T,unsigned int NDimension>
const typename SymRealSphericalHarmonicRep<T,NDimension>::RealValueType
SymRealSphericalHarmonicRep<T,NDimension>
  ::Evaluate(GradientDirectionType Gradient) const
{
  double theta = acos(Gradient[2]);
  double phi   = atan2(Gradient[1],Gradient[0]); // atan2(y,x) = atan(y/x);

  return (*this).Evaluate(theta,phi);
}

template<class T,unsigned int NDimension>
const unsigned int 
SymRealSphericalHarmonicRep<T,NDimension>
::GetJ(int l,int m)
{
  return 1 + m + l * (l+1) / 2;
}

template<class T,unsigned int NDimension>
const typename SymRealSphericalHarmonicRep<T,NDimension>::LmVector
SymRealSphericalHarmonicRep<T,NDimension>
::GetLM(unsigned int j)
{
  const int l = 2 * (int) ( ((1 + vcl_sqrt(8 * j - 7)) / 2) / 2);
  const int m = j - 1 - l * (l+1) / 2;
  LmVector retVal;

  retVal[0] = l;
  retVal[1] = m;

  return retVal;
}

/*
template<class T,unsigned int NDimension,class B>
template <typename TMatrixValueType>
SymRealSphericalHarmonicRep<T,NDimension,B>
SymRealSphericalHarmonicRep<T,NDimension,B>
::Rotate( const Matrix<TMatrixValueType, 3u, 3u> & m )
{

  Self result;

  const RshRotationMatixType rotMat = BasisType::Instance().ComputeRotationMatrix(m);
  
  typedef typename NumericTraits<T>::AccumulateType  AccumulateType;
  for(unsigned int r=0; r<itkGetStaticConstMacro(Dimension); r++)
  {
    AccumulateType sum = NumericTraits<AccumulateType>::ZeroValue();
    for(unsigned int c=0; c<itkGetStaticConstMacro(Dimension); c++)
    {
      sum += rotMat(r,c) * (*this)[c];
    }
    result[r] = static_cast<T>( sum );
  }
  
  return result;
}
*/

template<class T,unsigned int NDimension>
const 
typename SymRealSphericalHarmonicRep<T,NDimension>::RshBasisMatrixType
SymRealSphericalHarmonicRep<T,NDimension>
::ComputeRshBasis( const GradientDirectionContainerType *gradContainer ) 
{

  RshBasisMatrixType basis;
  std::vector<unsigned int> gradientind;
  for(GradientDirectionContainerType::ConstIterator gdcit = gradContainer->Begin();
      gdcit != gradContainer->End(); ++gdcit)
  {
    if(gdcit.Value().one_norm() > 0.0)
    {
      gradientind.push_back(gdcit.Index());
    }
  }

  unsigned int NumberOfCoefficients = Dimension;
  unsigned int numGrads = gradientind.size();

  basis.set_size( numGrads, NumberOfCoefficients );

  for (unsigned int m = 0; m < numGrads; m++)
  {
    /*** Grad directions relation to theta phi
    * x = sin(theta) * cos(phi)
    * y = sin(theta) * sin(phi)
    * z = cos(theta)
    */
    
    GradientDirectionType dir = gradContainer->ElementAt(gradientind[m]);
    dir = dir / dir.two_norm();
    
    double theta, phi;

    if ( dir[2] == 1) // z = 1
    {
      theta =0.0;
      phi   =0.0;
    }
    else
    {
      theta = acos(dir[2]);
      phi   = atan2(dir[1], dir[0]); // atan2(y,x) = atan(y/x);
    }

    for (unsigned int c = 0; c < NumberOfCoefficients; c++)
      basis[m][c]  = Y(c+1,theta,phi);

  }

  return basis;
}

/**
 * Compute the real spherical harmonics the traditional way.
 * Not sure how this stacks up agains other computation stratagies.
 */
template<class T,unsigned int NDimension>
const double
SymRealSphericalHarmonicRep<T,NDimension>
::Y( int j, double theta, double phi ) 
{

  LmVector vec = GetLM(j);
  const int l = vec[0];
  const int m = vec[1];
  
  if( m == 0 ) /// Y_l^0
    return K(l,0) * LegendreP(l,m,vcl_cos(theta));
  else if( m < 0 ) /// sqrt2 re(y_l^m)
    return vnl_math::sqrt2 * K(l,m) * vcl_cos(m*phi) * LegendreP(l,m,vcl_cos(theta));
  else ///(m > 0) sqrt2 im(y_l^m)
    return vnl_math::sqrt2* K(l,m) * vcl_sin(m*phi) * LegendreP(l,m,vcl_cos(theta));
}

/**
 * Nomalization factor for the spherical harmonics...
 * vcl_sqrt( ( (2*l+1) * factorial(l-m) ) / ( 4*( vnl_math::pi ) * factorial(l+m) ) );
 * 
 * Use a speed up to compute factorial(l-m) / factorial(l+m)
 * 
 * No Overflow is checked but this should be more robust then using the
 * Factorial method.
 */
template<class T,unsigned int NDimension>
const double
SymRealSphericalHarmonicRep<T,NDimension>
::K( int l, int m )
{
  double f = 1; //if m=0
  if (m > 0)
  {
    for(int i=l-m+1; i<l+m+1; i++)
    {
      f /= i; 
    }
  }
  else
  {
    for(int i=l+m+1; i<l-m+1; i++)
      {
        f *= i; 
      }
  }
  return vcl_sqrt( ( (2*l+1) / ( 4*( vnl_math::pi ) ) * f ) );
}

} // end namespace itk

#endif
