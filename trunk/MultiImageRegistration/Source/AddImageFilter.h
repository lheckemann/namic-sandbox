/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSubtractImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2006/04/03 15:07:51 $
  Version:   $Revision: 1.25 $

  Copyright (c) Insight Software  Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __AddImageFilter_h
#define __AddImageFilter_h

#include "itkBinaryFunctorImageFilter.h"

namespace itk
{
  
/** \class SubtractImageFilter
 * \brief Implements an operator for pixel-wise subtraction of two images.
 *
 * Output = Input1 - Input2.
 * 
 * This class is parametrized over the types of the two 
 * input images and the type of the output image. 
 * Numeric conversions (castings) are done by the C++ defaults.
 *
 * \ingroup IntensityImageFilters Multithreaded
 */
namespace Function {  
  
template< class TInput1, class TInput2, class TOutput>
class Add2
{
public:
  Add2() {}
  ~Add2() {}
  bool operator!=( const Add2 & ) const
  {
    return false;
  }
  bool operator==( const Add2 & other ) const
  {
    return !(*this != other);
  }
  inline TOutput operator()( const TInput1 & A, const TInput2 & B)
  { return (TOutput)(A + B); }
}; 
}

template <class TInputImage1, class TInputImage2, class TOutputImage>
class ITK_EXPORT AddImageFilter :
    public
BinaryFunctorImageFilter<TInputImage1,TInputImage2,TOutputImage, 
                         Function::Add2< 
  typename TInputImage1::PixelType, 
  typename TInputImage2::PixelType,
  typename TOutputImage::PixelType>   >
{
public:
  /** Standard class typedefs. */
  typedef AddImageFilter  Self;
  typedef BinaryFunctorImageFilter<TInputImage1,TInputImage2,TOutputImage, 
                                   Function::Add2< typename TInputImage1::PixelType,
                                                   typename TInputImage2::PixelType,
                                                   typename TOutputImage::PixelType> >  Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;


  /** Method for creation through the object factory. */
  itkNewMacro(Self);

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro(Input1Input2OutputAdditiveOperatorsCheck,
    (Concept::AdditiveOperators<typename TInputImage1::PixelType,
                                typename TInputImage2::PixelType,
                                typename TOutputImage::PixelType>));
  /** End concept checking */
#endif

protected:
  AddImageFilter() {}
  virtual ~AddImageFilter() {}

private:
  AddImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented


};

} // end namespace itk


#endif
