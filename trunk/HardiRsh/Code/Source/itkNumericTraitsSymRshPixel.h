
#ifndef __itkNumericTraitsSymRshPixel_h
#define __itkNumericTraitsSymRshPixel_h

#include "itkNumericTraits.h"
#include "itkSymRealSphericalHarmonicRep.h"

// This file is meant to define numeric traits for tensor pixels types in itk

namespace itk
{
template< typename T, unsigned int D >
class NumericTraits< SymRealSphericalHarmonicRep<T, D> >
{
public:
  typedef T                                                 ValueType;
  typedef SymRealSphericalHarmonicRep<typename NumericTraits<T>::PrintType,D>
                                                            PrintType;
  typedef SymRealSphericalHarmonicRep<typename NumericTraits<T>::AbsType,D>
                                                            AbsType;
  typedef SymRealSphericalHarmonicRep<typename NumericTraits<T>::AccumulateType,D>
                                                            AccumulateType;
  typedef SymRealSphericalHarmonicRep<typename NumericTraits<T>::RealType,D>
                                                            RealType;
  typedef typename NumericTraits<T>::RealType                        ScalarRealType;
  typedef SymRealSphericalHarmonicRep<typename NumericTraits<T>::FloatType,D>
                                                            FloatType;
  static SymRealSphericalHarmonicRep<T,D>  ITKCommon_EXPORT ZeroValue()
  {
    return SymRealSphericalHarmonicRep<T,D>( NumericTraits<T>::Zero );
  }
  static SymRealSphericalHarmonicRep<T,D>  ITKCommon_EXPORT OneValue()
  {
    return SymRealSphericalHarmonicRep<T,D>( NumericTraits<T>::One );
  }

  static const SymRealSphericalHarmonicRep<T,D> ITKCommon_EXPORT
                                                            Zero;
  static const SymRealSphericalHarmonicRep<T,D> ITKCommon_EXPORT
                                                            One;

};

template< typename T, unsigned int D >
const SymRealSphericalHarmonicRep<T,D>
NumericTraits< SymRealSphericalHarmonicRep<T, D> >
::Zero = SymRealSphericalHarmonicRep<T,D>( NumericTraits<T>::Zero );

template< typename T, unsigned int D >
const SymRealSphericalHarmonicRep<T,D>
NumericTraits< SymRealSphericalHarmonicRep<T, D> >
::One  = SymRealSphericalHarmonicRep<T,D>( NumericTraits<T>::One );

} // end namespace itk
//#include "itkNumericTraitsSymRshPixel.txx"

#endif // __itkNumericTraitsSymRshPixel_h  
