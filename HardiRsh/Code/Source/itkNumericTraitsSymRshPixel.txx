#include "itkNumericTraitsSymRshPixel.h"

namespace itk
{
//
// Helper macro for defining the traits of the Vector type for a given component type and dimension
//
#define SYM_RSH_STATIC_TRAITS_MACRO( T, N ) \
const SymRealSphericalHarmonicRep<T,N> \
  NumericTraits<SymRealSphericalHarmonicRep<T,N> >::Zero = \
  SymRealSphericalHarmonicRep<T,N>( NumericTraits<T>::Zero ); \
const SymRealSphericalHarmonicRep<T,N>  \
  NumericTraits<SymRealSphericalHarmonicRep<T,N> >::One  = \
  SymRealSphericalHarmonicRep<T,N>( NumericTraits<T>::One ); \
SymRealSphericalHarmonicRep<T,N> \
  NumericTraits<SymRealSphericalHarmonicRep<T,N> >::ZeroValue() \
  { return NumericTraits<SymRealSphericalHarmonicRep<T,N> >::Zero; } \
SymRealSphericalHarmonicRep<T,N> \
  NumericTraits<SymRealSphericalHarmonicRep<T,N> >::OneValue() \
  { return NumericTraits<SymRealSphericalHarmonicRep<T,N> >::One; }
  

//
// Helper macro for defining the traits of Vector<T,k> for k in [1:9]
//
#define SYM_RSH_STATIC_TRAITS_MACRO_FOR_ONE_TYPE(T) \
SYM_RSH_STATIC_TRAITS_MACRO(T,2 ); \
SYM_RSH_STATIC_TRAITS_MACRO(T,4 ); \
SYM_RSH_STATIC_TRAITS_MACRO(T,6 ); \
SYM_RSH_STATIC_TRAITS_MACRO(T,8 ); \
SYM_RSH_STATIC_TRAITS_MACRO(T,10 ); 

//
// Calls to macros for defining the traits of Vector for different component types
//
/*
SYM_RSH_STATIC_TRAITS_MACRO_FOR_ONE_TYPE( unsigned char );
SYM_RSH_STATIC_TRAITS_MACRO_FOR_ONE_TYPE( signed char );
SYM_RSH_STATIC_TRAITS_MACRO_FOR_ONE_TYPE( char );
SYM_RSH_STATIC_TRAITS_MACRO_FOR_ONE_TYPE( short );
SYM_RSH_STATIC_TRAITS_MACRO_FOR_ONE_TYPE( unsigned short );
SYM_RSH_STATIC_TRAITS_MACRO_FOR_ONE_TYPE( int );
SYM_RSH_STATIC_TRAITS_MACRO_FOR_ONE_TYPE( unsigned int );
SYM_RSH_STATIC_TRAITS_MACRO_FOR_ONE_TYPE( long );
SYM_RSH_STATIC_TRAITS_MACRO_FOR_ONE_TYPE( unsigned long );
*/
SYM_RSH_STATIC_TRAITS_MACRO_FOR_ONE_TYPE( float );
SYM_RSH_STATIC_TRAITS_MACRO_FOR_ONE_TYPE( double );
SYM_RSH_STATIC_TRAITS_MACRO_FOR_ONE_TYPE( long double );  


} // end namespace itk

