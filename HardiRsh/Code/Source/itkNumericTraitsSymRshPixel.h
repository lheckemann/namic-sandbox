
#ifndef __itkNumericTraitsSymRshPixel_h
#define __itkNumericTraitsSymRshPixel_h

#include "itkNumericTraits.h"
#include "itkSymRealSphericalHarmonicRep.h"

// This file is meant to define numeric traits for tensor pixels types in itk

namespace itk
{
template< typename T, unsigned int D, typename B >
class NumericTraits< SymRealSphericalHarmonicRep<T, D, B> >
{
public:
  typedef T                                                 ValueType;
  typedef SymRealSphericalHarmonicRep<typename NumericTraits<T>::PrintType,D, B>
                                                            PrintType;
  typedef SymRealSphericalHarmonicRep<typename NumericTraits<T>::AbsType,D, B>
                                                            AbsType;
  typedef SymRealSphericalHarmonicRep<typename NumericTraits<T>::AccumulateType,D, B>
                                                            AccumulateType;
  typedef SymRealSphericalHarmonicRep<typename NumericTraits<T>::RealType,D, B>
                                                            RealType;
  typedef typename NumericTraits<T>::RealType                        ScalarRealType;
  typedef SymRealSphericalHarmonicRep<typename NumericTraits<T>::FloatType,D, B>
                                                            FloatType;
  static SymRealSphericalHarmonicRep<T,D,B>  ITKCommon_EXPORT ZeroValue()
  {
    return SymRealSphericalHarmonicRep<T,D,B>( NumericTraits<T>::Zero );
  }
  static SymRealSphericalHarmonicRep<T,D,B>  ITKCommon_EXPORT OneValue()
  {
    return SymRealSphericalHarmonicRep<T,D,B>( NumericTraits<T>::One );
  }

  static const SymRealSphericalHarmonicRep<T,D, B> ITKCommon_EXPORT
                                                            Zero;
  static const SymRealSphericalHarmonicRep<T,D, B> ITKCommon_EXPORT
                                                            One;

};

template< typename T, unsigned int D, typename B >
const SymRealSphericalHarmonicRep<T,D,B>
NumericTraits< SymRealSphericalHarmonicRep<T, D, B> >
::Zero = SymRealSphericalHarmonicRep<T,D,B>( NumericTraits<T>::Zero );

template< typename T, unsigned int D, typename B >
const SymRealSphericalHarmonicRep<T,D,B>
NumericTraits< SymRealSphericalHarmonicRep<T, D, B> >
::One  = SymRealSphericalHarmonicRep<T,D,B>( NumericTraits<T>::One );

} // end namespace itk
//#include "itkNumericTraitsSymRshPixel.txx"

#endif // __itkNumericTraitsSymRshPixel_h  
