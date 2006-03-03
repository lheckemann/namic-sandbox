/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSqrtImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2006/01/23 17:55:48 $
  Version:   $Revision: 1.18 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSqrtImageFilter_h
#define __itkSqrtImageFilter_h

#include "itkUnaryFunctorImageFilter.h"
#include "vnl/vnl_math.h"

namespace itk
{
  
/** \class SqrtImageFilter
 * \brief Computes the sqrt(x) pixel-wise
 *
 * \ingroup IntensityImageFilters  Multithreaded
 */
namespace Function {  
  
template< class TInput, class TOutput>
class Sqrt
{
public:
  Sqrt() {}
  ~Sqrt() {}
  bool operator!=( const Sqrt & ) const
  {
    return false;
  }
  bool operator==( const Sqrt & other ) const
  {
    return !(*this != other);
  }
  inline TOutput operator()( const TInput & A )
  {
    return (TOutput)sqrt((double)A);
  }
}; 
}
template <class TInputImage, class TOutputImage>
class ITK_EXPORT SqrtImageFilter :
    public
UnaryFunctorImageFilter<TInputImage,TOutputImage, 
                        Function::Sqrt< typename TInputImage::PixelType, 
                                        typename TOutputImage::PixelType>   >
{
public:
  /** Standard class typedefs. */
  typedef SqrtImageFilter  Self;
  typedef UnaryFunctorImageFilter<TInputImage,TOutputImage, 
                                  Function::Sqrt< typename TInputImage::PixelType, 
                                                  typename TOutputImage::PixelType> >  Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Begin concept checking */
  itkConceptMacro(InputConvertibleToDoubleCheck,
                  (Concept::Convertible<InputImagePixelType, double>));
  itkConceptMacro(DoubleConvertibleToOutputCheck,
                  (Concept::Convertible<double, OutputImagePixelType>));
  /** End concept checking */

protected:
  SqrtImageFilter() {}
  virtual ~SqrtImageFilter() {}

private:
  SqrtImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace itk


#endif
