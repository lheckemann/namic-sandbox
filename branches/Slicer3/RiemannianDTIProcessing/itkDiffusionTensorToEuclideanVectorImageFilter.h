/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkExpImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2003/09/10 14:28:47 $
  Version:   $Revision: 1.16 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkDiffusionTensorToEuclideanVectorImageFilter_h
#define __itkDiffusionTensorToEuclideanVectorImageFilter_h

#include "itkUnaryFunctorImageFilter.h"
#include "vnl/vnl_math.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1300)
#pragma function(exp)
#endif

namespace itk
{
  
/** \class DiffusionTensorToEuclideanVectorImageFilter
 * \brief Converts DiffusionTensor3D pixels to Euclidean vectors by
 * weighting the off-diagonal components by \sqrt(2).
 * 
 * \ingroup IntensityImageFilters  Multithreaded
 *
 */

namespace Function {  
template< class TInput, class TOutput>
class DiffusionTensor3DToEuclideanVector
{
public:
  DiffusionTensor3DToEuclideanVector() {};
  ~DiffusionTensor3DToEuclideanVector() {};
  inline TOutput operator()( const TInput & A )
  {
    TOutput outPixel;
    int i, nextDiagonal, diagonalUpdate;
    const unsigned int inputDimension = TInput::Dimension;
    const unsigned int outputDimension = TOutput::Dimension;

    //std::cout << "Input:" << std::endl << A << std::endl;

    nextDiagonal = 0;
    diagonalUpdate = inputDimension;
    for(i = 0; i < outputDimension; i++)
    {
      if(i == nextDiagonal)
      {
        outPixel[i] = A[i];
        nextDiagonal += diagonalUpdate;
        diagonalUpdate--;
      }
      else
        outPixel[i] = A[i] * M_SQRT2;
    }

    //std::cout << "Output:" << std::endl << outPixel << std::endl;

    return outPixel;
  }
}; 
}
template <class TInputImage, class TOutputImage>
class ITK_EXPORT DiffusionTensorToEuclideanVectorImageFilter :
    public
UnaryFunctorImageFilter<TInputImage,TOutputImage, 
                        Function::DiffusionTensor3DToEuclideanVector< 
  typename TInputImage::PixelType, 
  typename TOutputImage::PixelType>   >
{
public:
  /** Standard class typedefs. */
  typedef DiffusionTensorToEuclideanVectorImageFilter  Self;
  typedef UnaryFunctorImageFilter<TInputImage,TOutputImage, 
                                  Function::DiffusionTensor3DToEuclideanVector< typename TInputImage::PixelType, 
                                                 typename TOutputImage::PixelType> >  Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
protected:
  DiffusionTensorToEuclideanVectorImageFilter() {}
  virtual ~DiffusionTensorToEuclideanVectorImageFilter() {}

private:
  DiffusionTensorToEuclideanVectorImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented


};

} // end namespace itk


#endif
