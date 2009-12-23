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
template<class T,unsigned int NDimension,class B>
SymRealSphericalHarmonicRep<T,NDimension,B>&
SymRealSphericalHarmonicRep<T,NDimension,B>
::operator= (const Self& r)
{
  BaseArray::operator=(r);
  return *this;
}


/**
 * Assignment Operator from a scalar constant
 */
template<class T,unsigned int NDimension,class B>
SymRealSphericalHarmonicRep<T,NDimension,B>&
SymRealSphericalHarmonicRep<T,NDimension,B>
::operator= (const ComponentType & r)
{
  BaseArray::operator=(&r);
  return *this;
}

/**
 * Assigment from a plain array
 */
template<class T,unsigned int NDimension,class B>
SymRealSphericalHarmonicRep<T,NDimension,B>&
SymRealSphericalHarmonicRep<T,NDimension,B>
::operator= (const ComponentArrayType r )
{
  BaseArray::operator=(r);
  return *this;
}


/**
 * Returns a temporary copy of a vector
 */
template<class T,unsigned int NDimension,class B>
SymRealSphericalHarmonicRep<T,NDimension,B>
SymRealSphericalHarmonicRep<T,NDimension,B>
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
template<class T,unsigned int NDimension,class B>
SymRealSphericalHarmonicRep<T,NDimension,B>
SymRealSphericalHarmonicRep<T,NDimension,B>
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
template<class T,unsigned int NDimension,class B>
const SymRealSphericalHarmonicRep<T,NDimension,B> &
SymRealSphericalHarmonicRep<T,NDimension,B>
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
template<class T,unsigned int NDimension,class B>
const SymRealSphericalHarmonicRep<T,NDimension,B> &
SymRealSphericalHarmonicRep<T,NDimension,B>
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
template<class T,unsigned int NDimension,class B>
const SymRealSphericalHarmonicRep<T,NDimension,B> &
SymRealSphericalHarmonicRep<T,NDimension,B>
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
template<class T,unsigned int NDimension,class B>
const SymRealSphericalHarmonicRep<T,NDimension,B> &
SymRealSphericalHarmonicRep<T,NDimension,B>
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
template<class T,unsigned int NDimension,class B>
SymRealSphericalHarmonicRep<T,NDimension,B>
SymRealSphericalHarmonicRep<T,NDimension,B>
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
template<class T,unsigned int NDimension,class B>
SymRealSphericalHarmonicRep<T,NDimension,B>
SymRealSphericalHarmonicRep<T,NDimension,B>
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
template<class T,unsigned int NDimension,class B>
std::ostream &
operator<<(std::ostream& os,const SymRealSphericalHarmonicRep<T,NDimension,B> & c )
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
template<class T,unsigned int NDimension,class B>
std::istream &
operator>>(std::istream& is, SymRealSphericalHarmonicRep<T,NDimension,B> & dt )
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
template<class T,unsigned int NDimension,class B>
typename SymRealSphericalHarmonicRep<T,NDimension,B>::RealValueType
SymRealSphericalHarmonicRep<T,NDimension,B>
  ::Evaluate( RealValueType theta, RealValueType phi )
{
  RealValueType result = 0;
  for( unsigned int i=0; i<Dimension; i++)
  {
    result += (*this)[i] * BasisType::Y(i+1,theta,phi);
  }
  return result;
}

template<class T,unsigned int NDimension,class B>
typename SymRealSphericalHarmonicRep<T,NDimension,B>::RealValueType
SymRealSphericalHarmonicRep<T,NDimension,B>
  ::Evaluate(GradientDirectionType Gradient)
{
  double theta = acos(Gradient[2]);
  double phi   = atan2(Gradient[1],Gradient[0]); // atan2(y,x) = atan(y/x);

  return (*this).Evaluate(theta,phi);
}


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


template<class T,unsigned int NDimension,class B>
const 
typename SymRealSphericalHarmonicRep<T,NDimension,B>::RshBasisMatrixType
SymRealSphericalHarmonicRep<T,NDimension,B>
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
      basis[m][c]  = BasisType::Y(c+1,theta,phi);

  }

  return basis;
}



} // end namespace itk

#endif
