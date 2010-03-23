/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkQuaternion.txx,v $
  Language:  C++
  Date:      $Date: 2007/12/20 19:19:18 $
  Version:   $Revision: 1.28 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkQuaternion_txx
#define __itkQuaternion_txx

#include "itkQuaternion.h"
#include "itkVector.h" 
#include "itkNumericTraits.h" 
#include "itkExceptionObject.h"


namespace itk
{

/** Constructor to initialize entire vector to one value. */
template<class T>
Quaternion<T>
::Quaternion()
{
  m_X = NumericTraits<T>::Zero;
  m_Y = NumericTraits<T>::Zero;
  m_Z = NumericTraits<T>::Zero;
  m_W = NumericTraits<T>::One;
}

/** Copy Constructor */
template<class T>
Quaternion<T>
::Quaternion( const Self & v)
{
  m_X = v.m_X;
  m_Y = v.m_Y;
  m_Z = v.m_Z;
  m_W = v.m_W;
}

/** Assignment Operator */
template<class T>
const Quaternion<T> &
Quaternion<T>
::operator=( const Self & v)
{
  m_X = v.m_X;
  m_Y = v.m_Y;
  m_Z = v.m_Z;
  m_W = v.m_W;
  return *this;
}

/** Set to an identity transform */
template<class T>
void
Quaternion<T>
::SetIdentity()
{
  m_X = NumericTraits<T>::Zero;
  m_Y = NumericTraits<T>::Zero;
  m_Z = NumericTraits<T>::Zero;
  m_W = NumericTraits<T>::One;
}

/** Return a vnl_quaternion */
template<class T>
vnl_quaternion<T>
Quaternion<T>
::GetVnlQuaternion(void) const
{
  return vnl_quaternion<T>(m_X,m_Y,m_Z,m_W);
}

/** Assignment and Composition Operator */
template<class T>
const Quaternion<T> &
Quaternion<T>
::operator*=( const Self & v)
{
  const double mx =  m_W*v.m_X - m_Z*v.m_Y + m_Y*v.m_Z + m_X*v.m_W;
  const double my =  m_Z*v.m_X + m_W*v.m_Y - m_X*v.m_Z + m_Y*v.m_W;
  const double mz = -m_Y*v.m_X + m_X*v.m_Y + m_W*v.m_Z + m_Z*v.m_W;
  const double mw = -m_X*v.m_X - m_Y*v.m_Y - m_Z*v.m_Z + m_W*v.m_W;

  m_X = mx;
  m_Y = my;
  m_Z = mz;
  m_W = mw;

  return *this;
}

/** Composition Operator */
template<class T>
Quaternion<T> 
Quaternion<T>
::operator*( const Self & v) const
{
 
  Self result;

  result.m_X =  m_W*v.m_X - m_Z*v.m_Y + m_Y*v.m_Z + m_X*v.m_W;
  result.m_Y =  m_Z*v.m_X + m_W*v.m_Y - m_X*v.m_Z + m_Y*v.m_W;
  result.m_Z = -m_Y*v.m_X + m_X*v.m_Y + m_W*v.m_Z + m_Z*v.m_W;
  result.m_W = -m_X*v.m_X - m_Y*v.m_Y - m_Z*v.m_Z + m_W*v.m_W;

  return result;
}

/** Division and Assignment Operator */
template<class T>
const Quaternion<T> &
Quaternion<T>
::operator/=( const Self & v)
{
  const double mx = -m_W*v.m_X + m_Z*v.m_Y - m_Y*v.m_Z + m_X*v.m_W;
  const double my = -m_Z*v.m_X - m_W*v.m_Y + m_X*v.m_Z + m_Y*v.m_W;
  const double mz =  m_Y*v.m_X - m_X*v.m_Y - m_W*v.m_Z + m_Z*v.m_W;
  const double mw =  m_X*v.m_X + m_Y*v.m_Y + m_Z*v.m_Z + m_W*v.m_W;

  m_X = mx;
  m_Y = my;
  m_Z = mz;
  m_W = mw;

  return *this;
}

/** Division Operator  */
template<class T>
Quaternion<T> 
Quaternion<T>
::operator/( const Self & v) const
{
  Self result;

  result.m_X = -m_W*v.m_X + m_Z*v.m_Y - m_Y*v.m_Z + m_X*v.m_W;
  result.m_Y = -m_Z*v.m_X - m_W*v.m_Y + m_X*v.m_Z + m_Y*v.m_W;
  result.m_Z =  m_Y*v.m_X - m_X*v.m_Y - m_W*v.m_Z + m_Z*v.m_W;
  result.m_W =  m_X*v.m_X + m_Y*v.m_Y + m_Z*v.m_Z + m_W*v.m_W;

  return result;
}

/** Comparision operator */
template<class T>
bool
Quaternion<T>
::operator!=( const Self & v) const
{
  return !(*this == v);
}

/** Comparision operator */
template<class T>
bool
Quaternion<T>
::operator==( const Self & v) const
{
  // Evaluate the quaternion ratio between them
  Self ratio = *this * v.GetReciprocal();
  
  const typename itk::NumericTraits< T >::AccumulateType 
                                            square = ratio.m_W * ratio.m_W;
  
  const double epsilon = 1e-300;

  if( vcl_fabs(1.0f - square ) < epsilon )
    {
    return true;
    }

  return false;  

}

/** Get Conjugate */
template<class T>
Quaternion<T> 
Quaternion<T>
::GetConjugate( void ) const
{
  Self result;
  
  result.m_X = -m_X;
  result.m_Y = -m_Y;
  result.m_Z = -m_Z;
  result.m_W =  m_W;

  return result;
}
 
/** Get Reciprocal */
template<class T>
Quaternion<T> 
Quaternion<T>
::GetReciprocal( void ) const
{
  Self result;
  
  result.m_X = -m_X;
  result.m_Y = -m_Y;
  result.m_Z = -m_Z;
  result.m_W =  m_W;

  return result;
}
 
/** Get Tensor part */
template<class T>
typename Quaternion<T>::ValueType
Quaternion<T>
::GetTensor( void ) const
{
  
  const ValueType tensor = 
    static_cast< ValueType > (
      vcl_sqrt(m_X*m_X + m_Y*m_Y + m_Z*m_Z + m_W*m_W ) );

  return tensor;
}
 
/** Normalize */
template<class T>
void
Quaternion<T>
::Normalize( void ) 
{
  const ValueType tensor = this->GetTensor();

  if( vcl_fabs(tensor ) < 1e-20 )
    {
    ExceptionObject except;
    except.SetDescription("Attempt to normalize a \
                           itk::Quaternion with zero tensor");
    except.SetLocation(__FILE__);
    throw except;
    }
  m_X /= tensor;
  m_Y /= tensor;
  m_Z /= tensor;
  m_W /= tensor;

}
 
/** Get Axis */
template<class T>
typename Quaternion<T>::VectorType 
Quaternion<T>
::GetAxis( void ) const
{
  VectorType axis;
  
  const RealType ax = static_cast<RealType>( m_X );
  const RealType ay = static_cast<RealType>( m_Y );
  const RealType az = static_cast<RealType>( m_Z );
  
  const RealType vectorNorm = vcl_sqrt(ax * ax  +  ay * ay  +  az * az  );

  if( vectorNorm == NumericTraits<RealType>::Zero )
    {
    axis[0] = NumericTraits<T>::Zero;
    axis[1] = NumericTraits<T>::Zero;
    axis[2] = NumericTraits<T>::Zero;
    }
  else 
    {
    axis[0] = m_X / vectorNorm;
    axis[1] = m_Y / vectorNorm;
    axis[2] = m_Z / vectorNorm;
    }

  return axis;
}
 
/** Get Right part */
template<class T>
typename Quaternion<T>::VectorType 
Quaternion<T>
::GetRight( void ) const
{
  VectorType axis;
  
  axis[0] = m_X;
  axis[1] = m_Y;
  axis[2] = m_Z;

  return axis;
}
 
/** Get Scalar part */
template<class T>
typename Quaternion<T>::ValueType 
Quaternion<T>
::GetScalar( void ) const
{
  return m_W;
}
 
/** Get Angle (in radians) */
template<class T>
typename Quaternion<T>::ValueType
Quaternion<T>
::GetAngle( void ) const
{
  const RealType ax = static_cast<RealType>( m_X );
  const RealType ay = static_cast<RealType>( m_Y );
  const RealType az = static_cast<RealType>( m_Z );
  
  const RealType vectorNorm = vcl_sqrt(ax * ax  +  ay * ay  +  az * az  );

  const ValueType angle = 2.0 * vcl_atan2(vectorNorm, static_cast<RealType>(m_W) );
  
  return angle;
}
 
/** Get the Square root of a quaternion */
template<class T>
Quaternion<T>
Quaternion<T>
::SquareRoot( void ) const
{
  const ValueType newScalar = vcl_sqrt( static_cast<double>( 1.0 + m_W ) );
  const double sqrtOfTwo    = vcl_sqrt( 2.0f );

  const double factor = 1.0f / ( newScalar * sqrtOfTwo );

  Self result;

  result.m_X = m_X * factor;
  result.m_Y = m_Y * factor;
  result.m_Z = m_Z * factor;
  result.m_W = newScalar / sqrtOfTwo;

  return result;
}

/** Compute the Exponential of the quaternion */
template<class T>
Quaternion<T>
Quaternion<T>
::Exponential( RealType exponent ) const
{
  Self result;
  RealType currentTensor = this->GetTensor();
  RealType newTensor = vcl_pow( currentTensor, exponent );

  RealType currentAngle = this->GetAngle();
  RealType newAngle = currentAngle * exponent;

  result.Set( this->GetAxis(), newAngle, newTensor );

  return result;
}
 
/** Set Axis and Angle (in radians) */
template<class T>
void
Quaternion<T>
::Set( const VectorType & axis, ValueType angle )
{
  const RealType vectorNorm = axis.GetNorm();

  const RealType cosangle2 = vcl_cos(angle / 2.0 );
  const RealType sinangle2 = vcl_sin(angle / 2.0 );
  
  const RealType factor = sinangle2 / vectorNorm;
  
  m_X = axis[0] * factor;
  m_Y = axis[1] * factor;
  m_Z = axis[2] * factor;
  
  m_W = cosangle2;
}
  
/** Set Axis,  Angle (in radians) and Tensor */
template<class T>
void
Quaternion<T>
::Set( const VectorType & axis, ValueType angle, ValueType tensor )
{
  const RealType vectorNorm = axis.GetNorm();

  const RealType cosangle2 = vcl_cos(angle / 2.0 );
  const RealType sinangle2 = vcl_sin(angle / 2.0 );
  
  const RealType factor = sinangle2 * tensor / vectorNorm;
  
  m_X = axis[0] * factor;
  m_Y = axis[1] * factor;
  m_Z = axis[2] * factor;
  
  m_W = cosangle2 * tensor;
}
 

/**  Set using an orthogonal matrix. */
template<class T>
void
Quaternion<T>
::Set( const MatrixType & mat )
{
  vnl_matrix<T> m( mat.GetVnlMatrix() );

  const double epsilon = 1e-30;

  double trace = m(0,0) + m(1,1) + m(2,2) + 1.0;

  if( trace > epsilon)
    {
    const double s = 0.5 / vcl_sqrt(trace);
    m_W = 0.25 / s;
    m_X = (m(2,1) - m(1,2)) * s;
    m_Y = (m(0,2) - m(2,0)) * s;
    m_Z = (m(1,0) - m(0,1)) * s;
    }
  else
    {
    if( m(0,0) > m(1,1) && m(0,0) > m(2,2) )
      {
      const double s = 2.0 * vcl_sqrt(1.0 + m(0,0) - m(1,1) - m(2,2));
      m_X = 0.25 * s;
      m_Y = (m(0,1) + m(1,0)) / s;
      m_Z = (m(0,2) + m(2,0)) / s;
      m_W = (m(1,2) - m(2,1)) / s;
      }
    else
      {
      if( m(1,1) > m(2,2) )
        {
        const double s = 2.0 * vcl_sqrt(1.0 + m(1,1) - m(0,0) - m(2,2));
        m_X = (m(0,1) + m(1,0)) / s;
        m_Y = 0.25 * s;
        m_Z = (m(1,2) + m(2,1)) / s;
        m_W = (m(0,2) - m(2,0)) / s;
        }
      else
        {
        const double s = 2.0 * vcl_sqrt(1.0 + m(2,2) - m(0,0) - m(1,1));
        m_X = (m(0,2) + m(2,0)) / s;
        m_Y = (m(1,2) + m(2,1)) / s;
        m_Z = 0.25 * s;
        m_W = (m(0,1) - m(1,0)) / s;
        }
      }
    }
}


/** Set right Part (in radians) */
template<class T>
void
Quaternion<T>
::Set( const VectorType & axis )
{

  const ValueType sinangle2 =  axis.GetNorm();
  if( sinangle2 > 1.0 )
    {
    ExceptionObject exception;
    exception.SetDescription("Trying to initialize a Quaternion with " \
                             "a vector whose magnitude is greater than 1");
    exception.SetLocation("itk::Quaternion::Set( const VectorType )");
    throw exception;
    }
  
  const ValueType cosangle2 =  vcl_sqrt(1.0 - sinangle2 * sinangle2 );
  
  m_X = axis[0];
  m_Y = axis[1];
  m_Z = axis[2];
  
  m_W = cosangle2;

}
 
/** Set the Quaternion from four components.
 *  After assignment, the quaternion is normalized
 *  in order to get a consistent Quaternion (unit quaternion). */
template<class T>
void
Quaternion<T>
::Set( T x, T y, T z, T w )
{
  //
  // We assume in this class that the W component is always non-negative.
  // The rotation represented by a Quaternion remains unchanged if all its 
  // four components are negated simultaneously. Therefore, if we are 
  // requested to initialize a Quaternion with a negative W, we negate the
  // signs of all the components.
  //
  if( w < 0.0 )
    {
    m_X = -x;
    m_Y = -y;
    m_Z = -z;
    m_W = -w;
    }
  else
    {
    m_X = x;
    m_Y = y;
    m_Z = z;
    m_W = w;
    }
}

/** Set from a vnl_quaternion
 *  After assignment, the quaternion is normalized
 *  in order to get a consistent Quaternion (unit quaternion). */
template<class T>
void
Quaternion<T>
::Set( const VnlQuaternionType & quaternion )
{
  m_X = quaternion.x();
  m_Y = quaternion.y();
  m_Z = quaternion.z();
  m_W = quaternion.r();
}

/** Set rotation around X axis */
template<class T>
void
Quaternion<T>
::SetRotationAroundX( ValueType angle )
{
  const ValueType sinangle2 = vcl_sin( angle / 2.0 );
  const ValueType cosangle2 = vcl_cos( angle / 2.0 );

  m_X = sinangle2;
  m_Y = NumericTraits< T >::Zero;
  m_Z = NumericTraits< T >::Zero;
  m_W = cosangle2;
}
 
/** Set rotation around Y axis  */
template<class T>
void
Quaternion<T>
::SetRotationAroundY( ValueType angle )
{
  const ValueType sinangle2 = vcl_sin( angle / 2.0 );
  const ValueType cosangle2 = vcl_cos( angle / 2.0 );

  m_X = NumericTraits< T >::Zero;
  m_Y = sinangle2;
  m_Z = NumericTraits< T >::Zero;
  m_W = cosangle2;
}
 
/**  Set rotation around Z axis  */
template<class T>
void
Quaternion<T>
::SetRotationAroundZ( ValueType angle )
{
  const ValueType sinangle2 = vcl_sin( angle / 2.0 );
  const ValueType cosangle2 = vcl_cos( angle / 2.0 );

  m_X = NumericTraits< T >::Zero;
  m_Y = NumericTraits< T >::Zero;
  m_Z = sinangle2;
  m_W = cosangle2;
}
 
/** Transform a Vector */
template<class T>
typename Quaternion<T>::VectorType 
Quaternion<T>
::Transform( const VectorType & v ) const
{
  VectorType result;
  
  const ValueType xx = m_X * m_X;
  const ValueType yy = m_Y * m_Y;
  const ValueType zz = m_Z * m_Z;
  const ValueType xy = m_X * m_Y;
  const ValueType xz = m_X * m_Z;
  const ValueType xw = m_X * m_W;
  const ValueType yz = m_Y * m_Z;
  const ValueType yw = m_Y * m_W;
  const ValueType zw = m_Z * m_W;

  const ValueType mxx = 1.0 - 2.0 * ( yy + zz );
  const ValueType myy = 1.0 - 2.0 * ( xx + zz );
  const ValueType mzz = 1.0 - 2.0 * ( xx + yy );
  const ValueType mxy = 2.0 * ( xy - zw );
  const ValueType mxz = 2.0 * ( xz + yw );
  const ValueType myx = 2.0 * ( xy + zw );
  const ValueType mzx = 2.0 * ( xz - yw );
  const ValueType mzy = 2.0 * ( yz + xw );
  const ValueType myz = 2.0 * ( yz - xw );
    
  result[0] = mxx * v[0] + mxy * v[1] + mxz * v[2];
  result[1] = myx * v[0] + myy * v[1] + myz * v[2];
  result[2] = mzx * v[0] + mzy * v[1] + mzz * v[2];

  return result;
}

/** Transform a CovariantVector
 *  given that this is an orthogonal transformation
 *  CovariantVectors are transformed as vectors. */
template<class T>
typename Quaternion<T>::CovariantVectorType 
Quaternion<T>
::Transform( const CovariantVectorType & v ) const
{
  CovariantVectorType result;
  
  const ValueType xx = m_X * m_X;
  const ValueType yy = m_Y * m_Y;
  const ValueType zz = m_Z * m_Z;
  const ValueType xy = m_X * m_Y;
  const ValueType xz = m_X * m_Z;
  const ValueType xw = m_X * m_W;
  const ValueType yz = m_Y * m_Z;
  const ValueType yw = m_Y * m_W;
  const ValueType zw = m_Z * m_W;

  const ValueType mxx = 1.0 - 2.0 * ( yy + zz );
  const ValueType myy = 1.0 - 2.0 * ( xx + zz );
  const ValueType mzz = 1.0 - 2.0 * ( xx + yy );
  const ValueType mxy = 2.0 * ( xy - zw );
  const ValueType mxz = 2.0 * ( xz + yw );
  const ValueType myx = 2.0 * ( xy + zw );
  const ValueType mzx = 2.0 * ( xz - yw );
  const ValueType mzy = 2.0 * ( yz + xw );
  const ValueType myz = 2.0 * ( yz - xw );
 
  result[0] = mxx * v[0] + mxy * v[1] + mxz * v[2];
  result[1] = myx * v[0] + myy * v[1] + myz * v[2];
  result[2] = mzx * v[0] + mzy * v[1] + mzz * v[2];

  return result;
}

/** Transform a Point */
template<class T>
typename Quaternion<T>::PointType 
Quaternion<T>
::Transform( const PointType & v ) const
{
  PointType result;
  
  const ValueType xx = m_X * m_X;
  const ValueType yy = m_Y * m_Y;
  const ValueType zz = m_Z * m_Z;
  const ValueType xy = m_X * m_Y;
  const ValueType xz = m_X * m_Z;
  const ValueType xw = m_X * m_W;
  const ValueType yz = m_Y * m_Z;
  const ValueType yw = m_Y * m_W;
  const ValueType zw = m_Z * m_W;
 
  const ValueType mxx = 1.0 - 2.0 * ( yy + zz );
  const ValueType myy = 1.0 - 2.0 * ( xx + zz );
  const ValueType mzz = 1.0 - 2.0 * ( xx + yy );
  const ValueType mxy = 2.0 * ( xy - zw );
  const ValueType mxz = 2.0 * ( xz + yw );
  const ValueType myx = 2.0 * ( xy + zw );
  const ValueType mzx = 2.0 * ( xz - yw );
  const ValueType mzy = 2.0 * ( yz + xw );
  const ValueType myz = 2.0 * ( yz - xw );
 
  result[0] = mxx * v[0] + mxy * v[1] + mxz * v[2];
  result[1] = myx * v[0] + myy * v[1] + myz * v[2];
  result[2] = mzx * v[0] + mzy * v[1] + mzz * v[2];

  return result;
}

/** Transform a VnlVector */
template<class T>
typename Quaternion<T>::VnlVectorType 
Quaternion<T>
::Transform( const VnlVectorType & v ) const
{
  VnlVectorType result;
  
  const ValueType xx = m_X * m_X;
  const ValueType yy = m_Y * m_Y;
  const ValueType zz = m_Z * m_Z;
  const ValueType xy = m_X * m_Y;
  const ValueType xz = m_X * m_Z;
  const ValueType xw = m_X * m_W;
  const ValueType yz = m_Y * m_Z;
  const ValueType yw = m_Y * m_W;
  const ValueType zw = m_Z * m_W;

  const ValueType mxx = 1.0 - 2.0 * ( yy + zz );
  const ValueType myy = 1.0 - 2.0 * ( xx + zz );
  const ValueType mzz = 1.0 - 2.0 * ( xx + yy );
  const ValueType mxy = 2.0 * ( xy - zw );
  const ValueType mxz = 2.0 * ( xz + yw );
  const ValueType myx = 2.0 * ( xy + zw );
  const ValueType mzx = 2.0 * ( xz - yw );
  const ValueType mzy = 2.0 * ( yz + xw );
  const ValueType myz = 2.0 * ( yz - xw );
    
  result[0] = mxx * v[0] + mxy * v[1] + mxz * v[2];
  result[1] = myx * v[0] + myy * v[1] + myz * v[2];
  result[2] = mzx * v[0] + mzy * v[1] + mzz * v[2];

  return result;
}
 
/** Get Matrix representation */
template<class T>
Matrix<T,3,3>
Quaternion<T>
::GetMatrix( void ) const
{
  Matrix<T,3,3> matrix;
  
  const ValueType xx = m_X * m_X;
  const ValueType yy = m_Y * m_Y;
  const ValueType zz = m_Z * m_Z;
  const ValueType xy = m_X * m_Y;
  const ValueType xz = m_X * m_Z;
  const ValueType xw = m_X * m_W;
  const ValueType yz = m_Y * m_Z;
  const ValueType yw = m_Y * m_W;
  const ValueType zw = m_Z * m_W;

  matrix[0][0] = 1.0 - 2.0 * ( yy + zz );
  matrix[1][1] = 1.0 - 2.0 * ( xx + zz );
  matrix[2][2] = 1.0 - 2.0 * ( xx + yy );
  matrix[0][1] = 2.0 * ( xy - zw );
  matrix[0][2] = 2.0 * ( xz + yw );
  matrix[1][0] = 2.0 * ( xy + zw );
  matrix[2][0] = 2.0 * ( xz - yw );
  matrix[2][1] = 2.0 * ( yz + xw );
  matrix[1][2] = 2.0 * ( yz - xw );
    
  return matrix;
}

} // end namespace itk

#endif