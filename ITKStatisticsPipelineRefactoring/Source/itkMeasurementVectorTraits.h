/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMeasurementVectorTraits.h,v $
  Language:  C++
  Date:      $Date: 2007/01/26 23:43:03 $
  Version:   $Revision: 1.6 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMeasurementVectorTraits_h
#define __itkMeasurementVectorTraits_h

#include "itkMacro.h"
#include "itkArray.h"
#include "itkVariableLengthVector.h"
#include "itkVector.h"
#include "itkFixedArray.h"
#include "vnl/vnl_vector_fixed.h"
#include "itkPoint.h"
#include "itkRGBPixel.h"
#include "itkMatrix.h"
#include "itkVariableSizeMatrix.h"
#include "itkNumericTraits.h"
#include "itkSize.h"
#include <vector>


namespace itk
{

/** \class MeasurementVectorTraits
 * \brief   
 * \ingroup Statistics 
 */


class MeasurementVectorTraits 
{
public:

  typedef unsigned int MeasurementVectorLength;
  
  template<class TValueType, unsigned int VLength>
  static void SetLength( FixedArray< TValueType, VLength > &m, const unsigned int s )
    {
    if( s != VLength )
      {
      itkGenericExceptionMacro( << "Cannot set the size of a FixedArray of length " 
          << VLength << " to " << s );
      }
    m.Fill( NumericTraits< TValueType >::Zero );
    }
  
  template<class TValueType, unsigned int VLength>
  static void SetLength( FixedArray< TValueType, VLength > *m, const unsigned int s )
    {
    if( s != VLength )
      {
      itkGenericExceptionMacro( << "Cannot set the size of a FixedArray of length " 
          << VLength << " to " << s );
      }
    m->Fill( NumericTraits< TValueType >::Zero );
    }
  
  template< class TValueType >
  static void SetLength( Array< TValueType > & m, const unsigned int s )
    {
    m.SetSize( s );
    m.Fill( NumericTraits< TValueType >::Zero );
    }
  
  template< class TValueType >
  static void SetLength( Array< TValueType > * m, const unsigned int s )
    {
    m->SetSize( s );
    m->Fill( NumericTraits< TValueType >::Zero );
    }

  template< class TValueType >
  static void SetLength( VariableLengthVector< TValueType > & m, const unsigned int s )
    {
    m.SetSize( s );
    m.Fill( NumericTraits< TValueType >::Zero );
    }
  
  template< class TValueType >
  static void SetLength( VariableLengthVector< TValueType > * m, const unsigned int s )
    {
    m->SetSize( s );
    m->Fill( NumericTraits< TValueType >::Zero );
    }

  template< class TValueType >
  static void SetLength( std::vector< TValueType > & m, const unsigned int s )
    {
    m.resize( s );
    }
  
  template< class TValueType >
  static void SetLength( std::vector< TValueType > * m, const unsigned int s )
    {
    m->resize( s );
    }


  template< class TValueType, unsigned int VLength > 
  static MeasurementVectorLength 
               GetLength( const FixedArray< TValueType, VLength > &)
    { return VLength; }
  
  template< class TValueType, unsigned int VLength > 
  static MeasurementVectorLength 
               GetLength( const FixedArray< TValueType, VLength > *)
    { return VLength; }

  template< class TValueType >
  static MeasurementVectorLength
               GetLength( const Array< TValueType > &m )
    {return m.GetSize(); }
  
  template< class TValueType >
  static MeasurementVectorLength
               GetLength( const Array< TValueType > *m )
    {return m->GetSize(); }

  template< class TValueType >
  static MeasurementVectorLength
               GetLength( const VariableLengthVector< TValueType > &m )
    {return m.GetSize(); }
  
  template< class TValueType >
  static MeasurementVectorLength
               GetLength( const VariableLengthVector< TValueType > *m )
    {return m->GetSize(); }

  template< class TValueType >
  static MeasurementVectorLength
               GetLength( const std::vector< TValueType > &m )
    {return m.size(); }
  
  template< class TValueType >
  static MeasurementVectorLength
               GetLength( const std::vector< TValueType > *m )
    {return m->size(); }


  template< class TValueType1, unsigned int VLength, class TValueType2 >
  static MeasurementVectorLength Assert( const FixedArray< TValueType1, VLength > &, 
                      const Array< TValueType2 > &b, const char *errMsg="Length Mismatch")
    {
    if( b.Size() == 0 )
      {
      return VLength;
      }
    if( b.Size() != 0 )
      {
      if (b.Size() != VLength)
        {
        itkGenericExceptionMacro( << errMsg );
        return 0;
        }
      }
    return 0;
    }

  template< class TValueType1, unsigned int VLength, class TValueType2 >
  static MeasurementVectorLength Assert( const FixedArray< TValueType1, VLength > *, 
                      const Array< TValueType2 > *b, const char *errMsg="Length Mismatch")
    {
    if( b->Size() == 0 )
      {
      return VLength;
      }
    else if (b->Size() != VLength)
      {
      itkGenericExceptionMacro( << errMsg );
      return 0;
      }
    return 0;
    }

  template< class TValueType1, unsigned int VLength, class TValueType2 >
  static MeasurementVectorLength Assert( const FixedArray< TValueType1, VLength > &, 
                      const VariableLengthVector< TValueType2 > &b, const char *errMsg="Length Mismatch")
    {
    if( b.Size() == 0 )
      {
      return VLength;
      }
    if( b.Size() != 0 )
      {
      if (b.Size() != VLength)
        {
        itkGenericExceptionMacro( << errMsg );
        return 0;
        }
      }
    return 0;
    }

  template< class TValueType1, unsigned int VLength, class TValueType2 >
  static MeasurementVectorLength Assert( const FixedArray< TValueType1, VLength > *, 
                      const VariableLengthVector< TValueType2 > *b, const char *errMsg="Length Mismatch")
    {
    if( b->Size() == 0 )
      {
      return VLength;
      }
    else if (b->Size() != VLength)
      {
      itkGenericExceptionMacro( << errMsg );
      return 0;
      }
    return 0;
    }

  template< class TValueType1, unsigned int VLength>
  static MeasurementVectorLength Assert( const FixedArray< TValueType1, VLength > &, 
                const MeasurementVectorLength l, const char *errMsg="Length Mismatch")
    {
    if( l == 0 )
      {
      return VLength;
      }
    else if( l != VLength )
      {
      itkGenericExceptionMacro( << errMsg );
      return 0;
      }
    return 0;
    }

  template< class TValueType1, unsigned int VLength>
  static MeasurementVectorLength Assert( const FixedArray< TValueType1, VLength > *, 
               const MeasurementVectorLength l, const char *errMsg="Length Mismatch")
    {
    if( l == 0 )
      {
      return VLength;
      }
    else if( l != VLength )
      {
      itkGenericExceptionMacro( << errMsg );
      return 0;
      }
    return 0;
    }

  template< class TValueType >
  static MeasurementVectorLength Assert( const Array< TValueType > &a, 
              const MeasurementVectorLength l, const char *errMsg="Length Mismatch")
    {
    if( ( ( l != 0 ) && ( a.Size() != l ) ) || ( a.Size() == 0 ) )
      {
      itkGenericExceptionMacro( << errMsg );
      }
    else if( l == 0 )
      {
      return a.Size();
      }
    return 0;
    }
  
  template< class TValueType >
  static MeasurementVectorLength Assert( const Array< TValueType > *a, 
              const MeasurementVectorLength l, const char *errMsg="Length Mismatch")
    {
    if( ( ( l != 0 ) && ( a->Size() != l )) || ( a->Size() == 0 ) )
      {
      itkGenericExceptionMacro( << errMsg );
      }
    else if( l == 0 )
      {
      return a->Size();
      }
    return 0;
    }
   
  template< class TValueType >
  static MeasurementVectorLength Assert( const VariableLengthVector< TValueType > &a, 
              const MeasurementVectorLength l, const char *errMsg="Length Mismatch")
    {
    if( ( ( l != 0 ) && ( a.Size() != l ) ) || ( a.Size() == 0 ) )
      {
      itkGenericExceptionMacro( << errMsg );
      }
    else if( l == 0 )
      {
      return a.Size();
      }
    return 0;
    }
  
  template< class TValueType >
  static MeasurementVectorLength Assert( const VariableLengthVector< TValueType > *a, 
              const MeasurementVectorLength l, const char *errMsg="Length Mismatch")
    {
    if( ( ( l != 0 ) && ( a->Size() != l ) ) || ( a->Size() == 0 ) )
      {
      itkGenericExceptionMacro( << errMsg );
      }
    else if( l == 0 )
      {
      return a->Size();
      }
    return 0;
    }
  template< class TValueType >
  static MeasurementVectorLength Assert( const std::vector< TValueType > &a, 
              const MeasurementVectorLength l, const char *errMsg="Length Mismatch")
    {
    if( ( ( l != 0 ) && ( a.size() != l ) ) || ( a.size() == 0 ) )
      {
      itkGenericExceptionMacro( << errMsg );
      }
    else if( l == 0 )
      {
      return a.size();
      }
    return 0;
    }
  
  template< class TValueType >
  static MeasurementVectorLength Assert( const std::vector< TValueType > *a, 
              const MeasurementVectorLength l, const char *errMsg="Length Mismatch")
    {
    if( ( ( l != 0 ) && ( a->size() != l ) ) || ( a->size() == 0 ) )
      {
      itkGenericExceptionMacro( << errMsg );
      }
    else if( l == 0 )
      {
      return a->size();
      }
    return 0;
    }

  template< class TArrayType >
  static void  Assign( TArrayType & m, const TArrayType & v )
    {
    m = v;
    }
 
  template< class TValueType, unsigned int VLength >
  static void  Assign( FixedArray< TValueType, VLength > & m, const TValueType & v )
    {
    m[0] = v;
    }
 
};

 
template<class TPixelType>
class MeasurementVectorPixelTraits
{
public:
  /* type of the vector that matches this pixel type */
  typedef TPixelType      MeasurementVectorType;
};


template<>
class MeasurementVectorPixelTraits<char>
{
public:
   typedef FixedArray< char, 1 >  MeasurementVectorType;
};

template<>
class MeasurementVectorPixelTraits<unsigned char>
{
public:
   typedef FixedArray< unsigned char, 1 >  MeasurementVectorType;
};

template<>
class MeasurementVectorPixelTraits<signed char>
{
public:
   typedef FixedArray< signed char, 1 >  MeasurementVectorType;
};

template<>
class MeasurementVectorPixelTraits<unsigned short>
{
public:
   typedef FixedArray< unsigned short, 1 >  MeasurementVectorType;
};

template<>
class MeasurementVectorPixelTraits<signed short>
{
public:
   typedef FixedArray< signed short, 1 >  MeasurementVectorType;
};

template<>
class MeasurementVectorPixelTraits<unsigned int>
{
public:
   typedef FixedArray< unsigned int, 1 >  MeasurementVectorType;
};

template<>
class MeasurementVectorPixelTraits<signed int>
{
public:
   typedef FixedArray< signed int, 1 >  MeasurementVectorType;
};

template<>
class MeasurementVectorPixelTraits<unsigned long>
{
public:
   typedef FixedArray< unsigned long, 1 >  MeasurementVectorType;
};

template<>
class MeasurementVectorPixelTraits<signed long>
{
public:
   typedef FixedArray< signed long, 1 >  MeasurementVectorType;
};

template<>
class MeasurementVectorPixelTraits<float>
{
public:
   typedef FixedArray< float, 1 >  MeasurementVectorType;
};

template<>
class MeasurementVectorPixelTraits<double>
{
public:
  typedef FixedArray< double, 1 >  MeasurementVectorType;
};


} // namespace itk

#endif  // __itkMeasurementVectorTraits_h
