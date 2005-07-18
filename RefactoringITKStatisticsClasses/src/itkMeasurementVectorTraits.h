#ifndef __itkMeasurementVectorTraits_h
#define __itkMeasurementVectorTraits_h

#include "itkMacro.h"
#include "itkArray.h"
#include "itkVector.h"
#include "itkFixedArray.h"
#include "itkSample.h"
#include "vnl/vnl_vector_fixed.h"
#include "itkPoint.h"
#include "itkRGBPixel.h"


namespace itk
{

/** \class MeasurementVectorTraits
 * \brief Define traits that set/return the length of a measurement vector.
 * This class uses traits to enforce consistency when dealing with measurement
 * vector lengths across different classes like std::vector, Array, Vector, 
 * FixedArray etc.
 *
 * The class is primarily intended to be used by developers adding statistics
 * classes to the toolkit.
 *
 * For instance, the developer can create a measurement vector as
 *
 * \code
 * typename SampleType:: MeasurementVectorType m_MeasurementVector 
 * = MeasurementVectorTraits< typename 
 *    SampleType::MeasurementVectorType >::SetSize( m_InputSample ) );
 * \endcode
 *
 * This creates \c m_MeasurementVector of a length that is the length of the 
 * measurement vectors contained in \c m_InputSample. If \c MeasurementVectorType
 * is a \c FixedArray, it will throw an exception if the measurement vectors in 
 * \c m_InputSample do not have the same length as the \c FixedArray. 
 *        Below, we set the create measurement vectors that copy the contents of 
 * the vector passed in.
 *
 * \code 
 * MeasurementVectorType m_MeasurementVector = MeasurementVectorTraits< 
 *                        MeasurementVectorType >::Create( m_StdVector );
 * MeasurementVectorType m_MeasurementVector = MeasurementVectorTraits<  
 *                        MeasurementVectorType >::Create( m_Array );
 * MeasurementVectorType m_MeasurementVector = MeasurementVectorTraits<  
 *                        MeasurementVectorType >::Create( m_FixedArray );
 * \endcode
 * 
 * 
 * Traits should be used to set the measurement vector size as an iVar of the 
 * class as below. This does the appropriate sanity checks.
 * 
 * \code 
 * this->SetMeasurementVectorSize( MeasurementVectorTraits< 
 *                        MeasurementVectorType >::Size( 4 ) );
 * \endcode
 * 
 * \ingroup Statistics 
 */

template <class T>
class MeasurementVectorTraits {
public:
  typedef T    MeasurementVectorType;
  static  T    SetSize( unsigned int );
  static unsigned int GetSize( const MeasurementVectorType * );
};

template<class TValueType, unsigned int TLength>
class MeasurementVectorTraits<FixedArray<TValueType, TLength > >
{
public:
  typedef TValueType ValueType;
  itkStaticConstMacro( MeasurementVectorLength, unsigned int, TLength );
  typedef FixedArray< ValueType, MeasurementVectorLength > 
                                        MeasurementVectorType;
  
  static MeasurementVectorType SetSize( unsigned int s )
    {
    if( s != MeasurementVectorLength )
      {
      itkGenericExceptionMacro( << "Cannot set the size of a FixedArray of length " 
          << MeasurementVectorLength << " to " << s );
      }
    MeasurementVectorType m;
    return m;
    }

  static unsigned int GetSize( const MeasurementVectorType *m )
    {
    return MeasurementVectorLength;
    }
};

template< class TValueType >
class MeasurementVectorTraits< Array< TValueType > >
{
public:
  typedef TValueType ValueType;
  typedef Array< ValueType > MeasurementVectorType;

  static MeasurementVectorType SetSize( unsigned int s )
    {
    MeasurementVectorType m( s );
    return m;
    }
  
  static unsigned int GetSize( const MeasurementVectorType *m )
    {
    if( m == NULL )
      {
      itkGenericExceptionMacro( << "MeasurementVector is NULL" );
      }
    return m->Size();
    }
};


template< class TValueType, unsigned int TLength >
class MeasurementVectorTraits< Vector< TValueType, TLength > >
{
public:
  typedef TValueType ValueType;
  itkStaticConstMacro( MeasurementVectorLength, unsigned int, TLength );
  
  typedef Vector< ValueType, MeasurementVectorLength > MeasurementVectorType;

  static MeasurementVectorType SetSize( unsigned int s )
    {
    if( s != MeasurementVectorLength )
      {
      itkGenericExceptionMacro( << "Cannot set the size of a Vector of length " 
          << MeasurementVectorLength << " to " << s );
      }
    MeasurementVectorType m;
    return m;
    }

  static unsigned int GetSize( const MeasurementVectorType *m )
    {
    return MeasurementVectorLength; 
    }
};

template< class TValueType >
class MeasurementVectorTraits< vnl_vector< TValueType > >
{
public:
  typedef TValueType ValueType;
  typedef vnl_vector< ValueType > MeasurementVectorType;

  static MeasurementVectorType SetSize( unsigned int s )
    {
    MeasurementVectorType m( s );
    return m;
    }
  
  static unsigned int GetSize( const MeasurementVectorType *m )
    {
    if( m == NULL )
      {
      itkGenericExceptionMacro( << "MeasurementVector is NULL" );
      }
    return m->size();
    }
};


template<class TValueType, unsigned int TLength>
class MeasurementVectorTraits< vnl_vector_fixed<TValueType, TLength > >
{
public:
  typedef TValueType ValueType;
  itkStaticConstMacro( MeasurementVectorLength, unsigned int, TLength );
  typedef vnl_vector_fixed< ValueType, MeasurementVectorLength > 
                                        MeasurementVectorType;
  
  static MeasurementVectorType SetSize( unsigned int s )
    {
    if( s != MeasurementVectorLength )
      {
      itkGenericExceptionMacro( << "Cannot set the size of a vnl_vector_fixed of length " 
          << MeasurementVectorLength << " to " << s );
      }
    MeasurementVectorType m;
    return m;
    }
  
  static unsigned int GetSize( const MeasurementVectorType *m )
    {
    return MeasurementVectorLength; 
    }
};

template< class TValueType >
class MeasurementVectorTraits< std::vector< TValueType > >
{
public:
  typedef TValueType ValueType;
  typedef std::vector< ValueType > MeasurementVectorType;

  static MeasurementVectorType SetSize( unsigned int s )
    {
    MeasurementVectorType m( s );
    return m;
    }
  
  static unsigned int GetSize( const MeasurementVectorType *m )
    {
    if( m == NULL )
      {
      itkGenericExceptionMacro( << "MeasurementVector is NULL" );
      }
    return m->size();
    }
};

template<class TValueType, unsigned int TLength>
class MeasurementVectorTraits< Point< TValueType, TLength > >
{
public:
  typedef TValueType ValueType;
  itkStaticConstMacro( MeasurementVectorLength, unsigned int, TLength );
  typedef Point< ValueType, MeasurementVectorLength > MeasurementVectorType;
  
  static MeasurementVectorType SetSize( unsigned int s )
    {
    if( s != MeasurementVectorLength )
      {
      itkGenericExceptionMacro( << "Cannot set the size of a Point of length " 
          << MeasurementVectorLength << " to " << s );
      }
    MeasurementVectorType m;
    return m;
    }

  static unsigned int GetSize( const MeasurementVectorType *m )
    {
    return MeasurementVectorLength;
    }
};

template<class TValueType >
class MeasurementVectorTraits< RGBPixel<TValueType > >
{
public:
  typedef TValueType ValueType;
  itkStaticConstMacro( MeasurementVectorLength, unsigned int, 3 );
  typedef RGBPixel< ValueType > MeasurementVectorType;
  
  static MeasurementVectorType SetSize( unsigned int s )
    {
    if( s != MeasurementVectorLength )
      {
      itkGenericExceptionMacro( << "Cannot set the size of an RGBPixel of length " 
          << MeasurementVectorLength << " to " << s );
      }
    MeasurementVectorType m;
    return m;
    }

  static unsigned int GetSize( const MeasurementVectorType *m )
    {
    return MeasurementVectorLength;
    }
};


} // __itkMeasurementVectorTraits_h

#endif
