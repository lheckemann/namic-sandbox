#ifndef _sbiaOrientationDistributionFunction_txx
#define _sbiaOrientationDistributionFunction_txx

#include "sbiaOrientationDistributionFunction.h"
//#include "itkNumericTraits.h"
#include "sbiaSymRealSphericalHarmonicRep.h"

namespace itk
{


/*
 * Default Constructor 
 */
template<class T,unsigned int NOrder>
OrientationDistributionFunction<T,NOrder>
::OrientationDistributionFunction():SymRealSphericalHarmonicRep<T,NOrder>()
{
}


/*
 * Constructor with initialization
 */
template<class T,unsigned int NOrder>
OrientationDistributionFunction<T,NOrder>
::OrientationDistributionFunction( const Self & r ):SymRealSphericalHarmonicRep<T,NOrder>(r)
{
}



/*
 * Constructor with initialization
 */
template<class T,unsigned int NOrder>
OrientationDistributionFunction<T,NOrder>
::OrientationDistributionFunction( const Superclass & r ):SymRealSphericalHarmonicRep<T,NOrder>(r)
{
}



/*
 * Constructor with initialization
 */
template<class T,unsigned int NOrder>
OrientationDistributionFunction<T,NOrder>
::OrientationDistributionFunction( const ComponentType & r ):SymRealSphericalHarmonicRep<T,NOrder>(r)
{
}




/*
 * Constructor with initialization
 */
template<class T,unsigned int NOrder>
OrientationDistributionFunction<T,NOrder>
::OrientationDistributionFunction( const ComponentArrayType r ):SymRealSphericalHarmonicRep<T,NOrder>(r)
{
}


/*
 * Assignment Operator
 */
template<class T,unsigned int NOrder>
OrientationDistributionFunction<T,NOrder>&
OrientationDistributionFunction<T,NOrder>
::operator= (const Self& r)
{
  Superclass::operator=(r);
  return *this;
}



/*
 * Assignment Operator
 */
template<class T,unsigned int NOrder>
OrientationDistributionFunction<T,NOrder>&
OrientationDistributionFunction<T,NOrder>
::operator= (const ComponentType & r)
{
  Superclass::operator=(r);
  return *this;
}



/*
 * Assignment Operator
 */
template<class T,unsigned int NOrder>
OrientationDistributionFunction<T,NOrder>&
OrientationDistributionFunction<T,NOrder>
::operator= (const ComponentArrayType r)
{
  Superclass::operator=(r);
  return *this;
}



/*
 * Assignment Operator
 */
template<class T,unsigned int NOrder>
OrientationDistributionFunction<T,NOrder>&
OrientationDistributionFunction<T,NOrder>
::operator= (const Superclass & r)
{
  Superclass::operator=(r);
  return *this;
}

/*
 * Assignment Operator
 */
template<class T,unsigned int NOrder>
    OrientationDistributionFunction<T,NOrder>&
        OrientationDistributionFunction<T,NOrder>
  ::operator= (const vnl_vector<ComponentType> r)
{
  Superclass::operator=(r);
  return *this;
}


/*
 * Normalize the coeffs to have unit power...
 */
template<class T,unsigned int NOrder>
void
OrientationDistributionFunction<T,NOrder>
  ::Normalize()
{
    //only normalize if 0th order is non zero...
  if ( (*this)[0] != 0)
  {
    double nFactor = 1.0 / ( 2 * sqrt( PI ) * (*this)[0]); 
    for( unsigned int i=0; i< Self::InternalDimension; i++) 
      (*this)[i] = nFactor * (*this)[i]; 
  }

}
  
/*
  * Ititlize the odf from the coeff of the rsh
 */
template<class T,unsigned int NOrder>
void
OrientationDistributionFunction<T,NOrder>
  ::InitFromDwiCoeffs( const vnl_vector<double> rshCoeffs )
{
    
  for( unsigned int i=0; i<Self::InternalDimension; i++){
    int l = (Self::getLM(i+1))[0];
    (*this)[i] = static_cast<T>( 2.0 * PI * Self::P( l , 0, 0 ) * rshCoeffs[i] ); 
  }
}

} // end namespace itk

#endif
