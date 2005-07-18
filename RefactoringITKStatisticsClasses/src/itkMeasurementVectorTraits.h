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
#include "itkMatrix.h"
#include "itkVariableSizeMatrix.h"


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
  static unsigned int GetSize();
};

template<class TValueType, unsigned int TLength>
class MeasurementVectorTraits<FixedArray<TValueType, TLength > >
{
public:
  typedef TValueType ValueType;
  typedef double     RealValueType;
  itkStaticConstMacro( MeasurementVectorLength, unsigned int, TLength );
  typedef FixedArray< ValueType, MeasurementVectorLength > 
                                        MeasurementVectorType;
  typedef FixedArray< RealValueType, 
          MeasurementVectorLength > RealMeasurementVectorType;
  typedef Matrix< RealValueType, MeasurementVectorLength, 
                                 MeasurementVectorLength > RealMatrixType;
  typedef Vector< RealValueType, MeasurementVectorLength > MeanType;
  typedef Vector< RealValueType, MeasurementVectorLength > OriginType;
  
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

  static unsigned int GetSize()
    {
    return MeasurementVectorLength;
    }

  static RealMatrixType RealMatrix( unsigned int r, unsigned int c )
    {
    RealMatrixType m;
    return m;
    }
};

template< class TValueType >
class MeasurementVectorTraits< Array< TValueType > >
{
public:
  typedef TValueType                   ValueType;
  typedef double                       RealValueType;
  typedef Array< ValueType >           MeasurementVectorType;
  typedef Array< RealValueType >       RealMeasurementVectorType;
  typedef VariableSizeMatrix< double > RealMatrixType;
  typedef RealMeasurementVectorType    MeanType;
  typedef RealMeasurementVectorType    OriginType;

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

  static unsigned int GetSize()
    {
    return 0;
    }

  static RealMatrixType RealMatrix( unsigned int r, unsigned int c )
    {
    RealMatrixType m;
    m.SetSize( r, c );
    return m;
    }
};


template< class TValueType, unsigned int TLength >
class MeasurementVectorTraits< Vector< TValueType, TLength > >
{
public:
  typedef TValueType ValueType;
  typedef double     RealValueType;
  itkStaticConstMacro( MeasurementVectorLength, unsigned int, TLength );
  typedef Vector< RealValueType, 
          MeasurementVectorLength > RealMeasurementVectorType;
  typedef Matrix< RealValueType, MeasurementVectorLength, 
                                 MeasurementVectorLength > RealMatrixType;
  typedef RealMeasurementVectorType                        MeanType;
  typedef RealMeasurementVectorType                        OriginType;
  typedef Vector< ValueType, MeasurementVectorLength >     MeasurementVectorType;

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

  static unsigned int GetSize()
    {
    return MeasurementVectorLength;
    }

  static RealMatrixType RealMatrix( unsigned int r, unsigned int c )
    {
    RealMatrixType m;
    return m;
    }
};

template< class TValueType >
class MeasurementVectorTraits< vnl_vector< TValueType > >
{
public:
  typedef TValueType                   ValueType;
  typedef double                       RealValueType;
  typedef vnl_vector< ValueType >      MeasurementVectorType;
  typedef vnl_vector< RealValueType >  RealMeasurementVectorType;
  typedef VariableSizeMatrix< double > RealMatrixType;
  typedef RealMeasurementVectorType    MeanType;
  typedef RealMeasurementVectorType    OriginType;

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

  static unsigned int GetSize()
    {
    return 0;
    }

  static RealMatrixType RealMatrix( unsigned int r, unsigned int c )
    {
    RealMatrixType m;
    m.SetSize( r, c );
    return m;
    }
};


template<class TValueType, unsigned int TLength>
class MeasurementVectorTraits< vnl_vector_fixed<TValueType, TLength > >
{
public:
  typedef TValueType ValueType;
  typedef double     RealValueType;
  itkStaticConstMacro( MeasurementVectorLength, unsigned int, TLength );
  typedef vnl_vector_fixed< ValueType, MeasurementVectorLength > 
                                        MeasurementVectorType;
  typedef vnl_vector_fixed< RealValueType, 
          MeasurementVectorLength > RealMeasurementVectorType;
  typedef Matrix< RealValueType, MeasurementVectorLength, 
                                 MeasurementVectorLength > RealMatrixType;
  typedef RealMeasurementVectorType                        MeanType;
  typedef Vector< RealValueType, MeasurementVectorLength > OriginType;
  
  
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

  static unsigned int GetSize()
    {
    return MeasurementVectorLength;
    }

  static RealMatrixType RealMatrix( unsigned int r, unsigned int c )
    {
    RealMatrixType m;
    return m;
    }
};

template< class TValueType >
class MeasurementVectorTraits< std::vector< TValueType > >
{
public:
  typedef TValueType ValueType;
  typedef double     RealValueType;
  typedef std::vector< ValueType > MeasurementVectorType;
  typedef std::vector< RealValueType > RealMeasurementVectorType;
  typedef VariableSizeMatrix< double > RealMatrixType;
  typedef RealMeasurementVectorType    MeanType;
  typedef RealMeasurementVectorType    OriginType;

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

  static unsigned int GetSize()
    {
    return 0;
    }

  static RealMatrixType RealMatrix( unsigned int r, unsigned int c )
    {
    RealMatrixType m;
    m.SetSize( r, c );
    return m;
    }
};

template<class TValueType, unsigned int TLength>
class MeasurementVectorTraits< Point< TValueType, TLength > >
{
public:
  typedef TValueType ValueType;
  typedef double     RealValueType;
  itkStaticConstMacro( MeasurementVectorLength, unsigned int, TLength );
  typedef Point< ValueType, MeasurementVectorLength > MeasurementVectorType;
  typedef Point< RealValueType, MeasurementVectorLength > RealMeasurementVectorType;
  typedef Matrix< RealValueType, MeasurementVectorLength, 
                                 MeasurementVectorLength > RealMatrixType;
  typedef Vector< RealValueType, MeasurementVectorLength>  MeanType;
  typedef Vector< RealValueType, MeasurementVectorLength>  OriginType;
  
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

  static unsigned int GetSize()
    {
    return 0;
    }

  static RealMatrixType RealMatrix( unsigned int r, unsigned int c )
    {
    RealMatrixType m;
    return m;
    }
};

template<class TValueType >
class MeasurementVectorTraits< RGBPixel<TValueType > >
{
public:
  typedef TValueType ValueType;
  typedef double     RealValueType;
  itkStaticConstMacro( MeasurementVectorLength, unsigned int, 3 );
  typedef RGBPixel< ValueType > MeasurementVectorType;
  typedef RGBPixel< RealValueType > RealMeasurementVectorType;
  typedef Matrix< RealValueType, MeasurementVectorLength, 
                                 MeasurementVectorLength > RealMatrixType;
  typedef Vector< RealValueType, MeasurementVectorLength > MeanType;
  typedef Vector< RealValueType, MeasurementVectorLength > OriginType;
  
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

  static unsigned int GetSize()
    {
    return MeasurementVectorLength;
    }

  static RealMatrixType RealMatrix( unsigned int r, unsigned int c )
    {
    RealMatrixType m;
    return m;
    }
};


} // __itkMeasurementVectorTraits_h

#endif
