/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkQuaternion.h,v $
  Language:  C++
  Date:      $Date: 2007/12/20 19:19:18 $
  Version:   $Revision: 1.22 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkQuaternion_h
#define __itkQuaternion_h

#include "itkVector.h"
#include "itkPoint.h"
#include "itkMatrix.h"
#include "itkCovariantVector.h"
#include "vnl/vnl_quaternion.h"
#include "vnl/vnl_vector_fixed.h"

namespace itk
{

/** \class Quaternion
 * \brief A templated class holding a unit quaternion.
 * 
 * Quaternion is a templated class that holds a unit quaternion.
 * The difference between versors and quaternions is that quaternions
 * can represent rotations and scale changes while versors are limited
 * to rotations.
 *
 * This class only implements the operations that maintain quaternions as a
 * group, that is, any operations between quaternions result in another
 * quaternion. This includes multiplication with scalars and quaternion
 * addition.
 * 
 * \ingroup Geometry
 * \ingroup DataRepresentation
 * 
 * \sa Vector
 * \sa Point
 * \sa CovariantVector
 * \sa Matrix
 * \sa Versor
 */
template<class T> 
class Quaternion 
{
public:
  /** Standard class typedefs. */
  typedef Quaternion  Self;
   
  /** ValueType can be used to declare a variable that is the same type
   * as a data element held in a Quaternion.   */
  typedef T ValueType;

  /** Type used for computations on the quaternion components */
  typedef typename NumericTraits<ValueType>::RealType    RealType;
  
  /** Vector type used to represent the axis. */
  typedef  Vector<T,3>   VectorType;

  /** Point type.  */
  typedef  Point<T,3>   PointType;
 
  /** CovariantVector type.  */
  typedef  CovariantVector<T,3>   CovariantVectorType;
 
  /** Vnl Vector type.  */
  typedef  vnl_vector_fixed<T,3>   VnlVectorType;

  /** Vnl Quaternion type.  */
  typedef  vnl_quaternion<T>       VnlQuaternionType;

  /** Type of the rotation matrix equivalent to the Quaternion */
  typedef  Matrix<T,3,3>          MatrixType;

  /** Get a vnl_quaternion with a copy of the internal memory block. */
  vnl_quaternion<T> GetVnlQuaternion( void ) const;

  /** Set the Quaternion from a Quaternion 
   \warning After assignment, the corresponding quaternion will 
            be normalized in order to get a consistent Quaternion.  */
  void Set( const VnlQuaternionType & ); 

  /** Set the Quaternion from Quaternion components. */
  void Set( T x, T y, T z, T w );  


  /** Default constructor creates an identity quaternion 
   * (representing 0 degrees  rotation). */
  Quaternion();

  /** Copy constructor.  */
  Quaternion(const Self & v);

  /** Assignment operator =.  Copy the quaternion argument. */
  const Self& operator=(const Self & v);
 
  /** Composition operator *=.  Compose the current quaternion
   * with the operand and store the result in the current
   * quaternion. */
  const Self& operator*=(const Self & v);

  /** Division operator /=.  Divide the current quaternion
   * with the operand and store the result in the current
   * quaternion. This is equivalent to compose the Quaternion with
   * the reciprocal of the operand \sa GetReciprocal */
  const Self& operator/=(const Self & v);


  /** Get Tensor part of the Quaternion.  Note that the term "Tensor" in
   * Hamilton's description of Quaternions is different from the notion of
   * Tensor as a higher rank representation of vectors. Tensor in the context
   * of Quaternions refers to the Scaling factor that the Quaternion applies to
   * a Vector. The Tensor of a Quaternion is a Scalar value. */
  ValueType GetTensor(void) const;

  /** Normalize the Quaternion.
   *  Reduces a Quaternion to a Versor. */
  void Normalize(void);

  /** Get Conjugate quaternion.  Returns the quaternion that produce a rotation
   * by the same angle but in opposite direction. The Tensor (scalar) part of
   * the Quaternion is conserved.  The conjugate of the quaternion 
   * Q= w + xi + * yj + zk is the quaternion Q' = w - xi - yj - zk */
  Self GetConjugate(void) const;

  /** Get Reciprocal quaternion.  Returns the quaternion that composed
   * with this one will result in a scalar operator equals to 1.
   * It is also equivalent to 1/this. */
  Self GetReciprocal(void) const;

  /** Quaternion operator*.  Performs the composition of two quaternions.
   * this operation is NOT commutative. */
  Self operator*(const Self &vec) const;

  /** Quaternion operator/.  Performs the division of two quaternions. */
  Self operator/(const Self &vec) const;

  /** Quaternion operator==  Performs the comparison between two quaternions.
   * this operation uses an arbitrary threshold for the comparison.  */
  bool operator==(const Self &vec) const;

  /** Quaternion operator!=  Performs the comparison between two quaternions.
   * this operation uses an arbitrary threshold for the comparison.  */
  bool operator!=(const Self &vec) const;

  /** Returns the Scalar part. */
  ValueType GetScalar( void ) const;

  /** Returns the X component. */
  ValueType GetX( void ) const
    { return m_X; }

  /** Returns the Y component. */
  ValueType GetY( void ) const
    { return m_Y; }

  /** Returns the Z component. */
  ValueType GetZ( void ) const
    { return m_Z; }

  /** Returns the W component. */
  ValueType GetW( void ) const
    { return m_W; }

  /** Returns the rotation angle in radians.  */
  ValueType GetAngle( void ) const;

  /** Returns the axis of the rotation.
   * It is a unit vector parallel to the axis. */
  VectorType GetAxis( void ) const;
   
  /** Returns the Right part
   * It is a vector part of the Quaternion. It is 
   * called Right because it is equivalent to
   * a right angle rotation. */
  VectorType GetRight( void ) const;
   
  /** Set the quaternion using a vector and angle
   * the unit vector parallel to the given vector 
   * will be used. The angle is expected in radians. */
  void Set( const VectorType & axis, ValueType angle );
  
  /** Set the quaternion using a vector, angle and tensor.
   * The unit vector parallel to the given vector 
   * will be used. The angle is expected in radians. */
  void Set( const VectorType & axis, ValueType angle, ValueType tensor );
 
  /** Set the quaternion using an orthogonal matrix.
   *  Based on code from:
   *  http://www.euclideanspace.com/maths/geometry/rotations/
   *  conversions/matrixToQuaternion/index.htm
   */
  void Set( const MatrixType & m );

  /** Set the quaternion using the right part.
   * the magnitude of the vector given is assumed to 
   * be equal to vcl_sin(angle/2).
   * This method will compute internally the scalar
   * part that preserve the Quaternion as a unit quaternion. */
  void Set( const VectorType & axis );

  /** Sets a rotation around the X axis using the parameter
   * as angle in radians. This is a method provided for 
   * convinience to initialize a rotation. The effect of
   * this methods is not cumulative with any value previously
   * stored in the Quaternion.
   * \sa Set \sa SetRotationAroundY \sa SetRotationAroundZ */
  void SetRotationAroundX( ValueType angle );
 
  /** Sets a rotation around the Y axis using the parameter
   * as angle in radians. This is a method provided for 
   * convinience to initialize a rotation. The effect of
   * this methods is not cumulative with any value previously
   * stored in the Quaternion.
   * \sa Set \sa SetRotationAroundX \sa SetRotationAroundZ */
  void SetRotationAroundY( ValueType angle );
 
  /** Sets a rotation around the Y axis using the parameter
   * as angle in radians. This is a method provided for 
   * convinience to initialize a rotation. The effect of
   * this methods is not cumulative with any value previously
   * stored in the Quaternion.
   * \sa Set \sa SetRotationAroundX \sa SetRotationAroundY */
  void SetRotationAroundZ( ValueType angle );

  /** Reset the values so the quaternion is equivalent to an identity 
   *  transformation. This is equivalent to set a zero angle */
  void SetIdentity();
  
  /** Transform a vector. */
  VectorType Transform( const VectorType & v ) const;

  /** Transform a covariant vector.  */
  CovariantVectorType Transform( const CovariantVectorType & v ) const;

  /** Transform a point. */
  PointType Transform( const PointType & v ) const;

  /** Transform a vnl_vector. */
  VnlVectorType Transform( const VnlVectorType & v ) const;

  /** Get the matrix representation. */
  MatrixType GetMatrix(void) const;
   
  /** Get the Square root of the unit quaternion. */
  Self SquareRoot(void) const;
   
  /** Compute the Exponential of the unit quaternion
   * Exponentiation by a factor is equivalent to 
   * multiplication of the rotation angle of the quaternion. */
  Self Exponential( RealType exponent ) const;

private: 
  /** Component parallel to x axis.  */
  ValueType  m_X;
   
  /** Component parallel to y axis.  */
  ValueType  m_Y;
   
  /** Component parallel to z axis.  */
  ValueType  m_Z;

  /** Escalar component of the Quaternion.  */
  ValueType  m_W;
};

template< class T>  
ITK_EXPORT std::ostream& operator<<( std::ostream& os, 
                                     const Quaternion<T> & v)
{
  os << "[ ";
  os << v.GetX() << ", " << v.GetY() << ", ";
  os << v.GetZ() << ", " << v.GetW() << " ]";
  return os;
}

template< class T>
ITK_EXPORT std::istream& operator>>(std::istream& is, 
                                    Quaternion<T> & v); 


} // end namespace itk
  

// Define instantiation macro for this template.
#define ITK_TEMPLATE_Quaternion(_, EXPORT, x, y) namespace itk { \
  _(1(class EXPORT Quaternion< ITK_TEMPLATE_1 x >)) \
  namespace Templates { typedef Quaternion< ITK_TEMPLATE_1 x > Quaternion##y; } \
  }

#if ITK_TEMPLATE_EXPLICIT
# include "Templates/itkQuaternion+-.h"
#endif

#if ITK_TEMPLATE_TXX
# include "itkQuaternion.txx"
#endif


#endif 
