/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkQuadraticQuaternionPolynomial.h,v $
  Language:  C++
  Date:      $Date: 2007/12/20 19:19:18 $
  Version:   $Revision: 1.22 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkQuadraticQuaternionPolynomial_h
#define __itkQuadraticQuaternionPolynomial_h

#include "itkQuaternion.h"
#include "itkFunctionBase.h"

namespace itk
{

/** \class QuadraticQuaternionPolynomial
 * \brief A templated class for computing a second order interpolation between three versors.
 * 
 * QuadraticQuaternionPolynomial is a templated class that holds three versors
 * and performs second order interpolation between them.
 *
 * \ingroup Geometry
 * \ingroup DataRepresentation
 * 
 * \sa Quaternion
 */
template<class T> 
class QuadraticQuaternionPolynomial : FunctionBase< Quaternion<T>, Quaternion<T> >
{
public:
  /** Standard class typedefs. */
  typedef QuadraticQuaternionPolynomial             Self;
  typedef FunctionBase< Quaternion<T>, Quaternion<T>  >   Superclass;
  typedef SmartPointer<Self>                      Pointer;
  typedef SmartPointer<const Self>                ConstPointer;

   
  /** Quaternion type to use as nodes of the polynomial. */
  typedef  Quaternion<T>    QuaternionType;

  /** Run-time type information (and related methods). */
  itkTypeMacro(QuadraticQuaternionPolynomial, FunctionBase);

  /** Input type */
  typedef typename Superclass::InputType    InputType;

  /** Output type */
  typedef typename Superclass::OutputType    OutputType;

  /** Evaluate at the specified input position */
  virtual OutputType Evaluate( const InputType& input ) const;

protected:
  /** Constructor and Destructor */
  QuadraticQuaternionPolynomial();
  virtual ~QuadraticQuaternionPolynomial();

  /** Set the three versors that will be used for the interpolation.
   *  The interpolation will be done in such a way that for values t:
   *
   *  t = 0.0  value = V0 
   *  t = 0.5  value = V0/4 + V1/2 + V2/4
   *  t = 1.0  value = V2 
   *
   *  */
  itkSetMacro( V0, QuaternionType );
  itkSetMacro( V1, QuaternionType );
  itkSetMacro( V2, QuaternionType );

private: 
  QuadraticQuaternionPolynomial(const Self& ); //purposely not implemented
  void operator=(const Self& ); //purposely not implemented

  /** Quaternion value */
  QuaternionType  m_V0;
  QuaternionType  m_V1;
  QuaternionType  m_V2;
};

template< class T>  
ITK_EXPORT std::ostream& operator<<( std::ostream& os, 
                                     const QuadraticQuaternionPolynomial<T> & interpolator )
{
  os << "V0 = " << interpolator->m_V0 << std::endl;
  os << "V0 = " << interpolator->m_V1 << std::endl;
  os << "V0 = " << interpolator->m_V2 << std::endl;
  return os;
}

template< class T>
ITK_EXPORT std::istream& operator>>(std::istream& is, 
                                    QuadraticQuaternionPolynomial<T> & v); 


} // end namespace itk
  

// Define instantiation macro for this template.
#define ITK_TEMPLATE_QuadraticQuaternionPolynomial(_, EXPORT, x, y) namespace itk { \
  _(1(class EXPORT QuadraticQuaternionPolynomial< ITK_TEMPLATE_1 x >)) \
  namespace Templates { typedef QuadraticQuaternionPolynomial< ITK_TEMPLATE_1 x > QuadraticQuaternionPolynomial##y; } \
  }

#if ITK_TEMPLATE_TXX
# include "itkQuadraticQuaternionPolynomial.txx"
#endif


#endif 
