/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkVersorQuadraticInterpolator.h,v $
  Language:  C++
  Date:      $Date: 2007/12/20 19:19:18 $
  Version:   $Revision: 1.22 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkVersorQuadraticInterpolator_h
#define __itkVersorQuadraticInterpolator_h

#include "itkVersor.h"

namespace itk
{

/** \class VersorQuadraticInterpolator
 * \brief A templated class for computing a second order interpolation between three versors.
 * 
 * VersorQuadraticInterpolator is a templated class that holds three versors
 * and performs second order interpolation between them.
 *
 * \ingroup Geometry
 * \ingroup DataRepresentation
 * 
 * \sa Versor
 */
template<class T> 
class VersorQuadraticInterpolator 
{
public:
  /** Standard class typedefs. */
  typedef VersorQuadraticInterpolator  Self;
   
  /** Versor type used to represent the axis. */
  typedef  Versor<T>    VersorType;

protected:
  /** Constructor and Destructor */
  VersorQuadraticInterpolator();
  virtual ~VersorQuadraticInterpolator();

private: 
  /** Versor value */
  VersorType  m_V0;
  VersorType  m_V1;
  VersorType  m_V2;
};

template< class T>  
ITK_EXPORT std::ostream& operator<<( std::ostream& os, 
                                     const VersorQuadraticInterpolator<T> & interpolator )
{
  os << "V0 = " << interpolator->m_V0 << std::endl;
  os << "V0 = " << interpolator->m_V1 << std::endl;
  os << "V0 = " << interpolator->m_V2 << std::endl;
  return os;
}

template< class T>
ITK_EXPORT std::istream& operator>>(std::istream& is, 
                                    VersorQuadraticInterpolator<T> & v); 


} // end namespace itk
  

// Define instantiation macro for this template.
#define ITK_TEMPLATE_VersorQuadraticInterpolator(_, EXPORT, x, y) namespace itk { \
  _(1(class EXPORT VersorQuadraticInterpolator< ITK_TEMPLATE_1 x >)) \
  namespace Templates { typedef VersorQuadraticInterpolator< ITK_TEMPLATE_1 x > VersorQuadraticInterpolator##y; } \
  }

#if ITK_TEMPLATE_TXX
# include "itkVersorQuadraticInterpolator.txx"
#endif


#endif 
